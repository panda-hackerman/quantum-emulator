//
// Created by Eli Michaud on 6/25/2026.
//

#include "custom_windows.h"

#include "imgui.h"

void CircuitEditorWindow::Draw() {
  ImGui::PushItemWidth(80);
  ImGui::InputInt("Qubits", &data.num_qubits);
  ImGui::SameLine();
  ImGui::InputInt("Layers", &data.num_layers);

  // Update Size
  UpdateCircuitSize();

  // Table
  constexpr ImGuiTableFlags table_flags =
      ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedFit;

  // Actually draw
  if (ImGui::BeginTable("Circuit Diagram", data.num_layers + 1, table_flags)) {

    // Setup Columns
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize);

    for (int i = 0; i < data.num_layers; ++i) {
      ImGui::TableSetupColumn(std::format("t{}", i).data(),
                              ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize);
    }

    ImGui::TableHeadersRow();

    // Loop over actual grid
    for (int qubit = 0; qubit < data.num_qubits; ++qubit) {

      { // Column 0 (labels)
        ImGui::PushID(qubit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("q%d", qubit);
      }

      // Column 1 to N+1
      for (int layer = 0; layer < data.num_layers; ++layer) {
        ImGui::PushID(layer);
        ImGui::TableSetColumnIndex(layer + 1);

        const int button_id = layer + (data.num_layers * qubit);
        const CircuitPart &part = circuit->GetCircuitPart(static_cast<Circuit::GridSize_T>(qubit),
                                                          static_cast<Circuit::GridSize_T>(layer));

        const char *name = part.name == nullptr ? "" : part.name;
        const std::string label = std::format("{}##{}", name, button_id);
        ImGui::Button(label.data(), ImVec2{60, 60});
        ImGui::PopID();
      }
      ImGui::PopID();
    }

    ImGui::EndTable();
  }
}

void CircuitEditorWindow::UpdateCircuitSize() {
  int &qubits = data.num_qubits;
  int &layers = data.num_layers;

  qubits = std::clamp<int>(qubits, gates::kMinCircuitQubits, gates::kMaxCircuitQubits);
  layers = std::clamp<int>(layers, gates::kMinCircuitDepth, gates::kMaxCircuitDepth);

  if (std::cmp_not_equal(qubits, circuit->NumQubits()) ||
      std::cmp_not_equal(layers, circuit->CircuitDepth())) {
    circuit->SetNewSize(static_cast<Circuit::GridSize_T>(qubits),
                        static_cast<Circuit::GridSize_T>(layers));
      }
}