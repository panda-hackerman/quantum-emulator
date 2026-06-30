//
// Created by Eli Michaud on 6/25/2026.
//

#include "custom_windows.h"

#include "imgui.h"
#include "math/simulator.h"

void CircuitEditor::Draw() {
  ImGui::PushItemWidth(80);
  ImGui::InputInt("Qubits", &data.num_qubits);
  ImGui::SameLine();
  ImGui::InputInt("Layers", &data.num_layers);
  ImGui::PopItemWidth();

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

        const GateButton *gate_button = buttons_arr_[qubit][layer];
        ImGui::Button(gate_button->name, ImVec2{60, 60});

        // DRAG AND DROP / SOURCE
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
          GateSwapPayload payload = {static_cast<Circuit::GridSize_T>(qubit),
                                     static_cast<Circuit::GridSize_T>(layer)};

          ImGui::SetDragDropPayload("BUTTON_SWAP", &payload, sizeof(payload));
          ImGui::Text("%s", "Move gate");
          ImGui::EndDragDropSource();
        }

        // DRAG AND DROP / TARGET
        if (ImGui::BeginDragDropTarget()) {
          if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("BUTTON_SWAP")) {
            auto [payload_qubit, payload_layer] = *static_cast<GateSwapPayload *>(payload->Data);

            Set(qubit, layer, buttons_arr_[payload_qubit][payload_layer]);
            Set(payload_qubit, payload_layer, gate_button);
          }

          if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("BUTTON_SET")) {
            const GateButton *payload_button = *static_cast<const GateButton **>(payload->Data);

            Set(qubit, layer, payload_button);
          }

          ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
      }

      ImGui::PopID();
    }

    ImGui::EndTable();
  }
}

void CircuitEditor::UpdateCircuitSize() {
  int &qubits = data.num_qubits;
  int &layers = data.num_layers;

  qubits = std::clamp<int>(qubits, Circuit::kMinQubits, Circuit::kMaxQubits);
  layers = std::clamp<int>(layers, Circuit::kMinDepth, Circuit::kMaxDepth);

  if (std::cmp_not_equal(qubits, circuit_->GetNumQubits()) ||
      std::cmp_not_equal(layers, circuit_->GetNumLayers())) {
    circuit_->SetSize(static_cast<Circuit::GridSize_T>(qubits),
                     static_cast<Circuit::GridSize_T>(layers));

    *circuit_dirty_ = true;
  }
}

void CircuitEditor::Set(const Circuit::GridSize_T qubit, const Circuit::GridSize_T layer,
                        const GateButton *button) {
  if (circuit_ == nullptr) return;

  switch (button->part_type) {
    case Circuit::Part::kEmpty:
      circuit_->AddEmpty(qubit, layer);
      break;
    case Circuit::Part::kMatrix2x2:
      circuit_->AddGate(qubit, layer, button->matrix);
      break;
    case Circuit::Part::kControlBit:
      circuit_->AddControlBit(qubit, layer);
      break;
    case Circuit::Part::kAntiControlBit:
      circuit_->AddAntiControlBit(qubit, layer);
      break;
    case Circuit::Part::kMeasure:
      circuit_->AddMeasurement(qubit, layer);
      break;
    case Circuit::Part::kSwap:
      circuit_->AddSwap(qubit, layer);
      break;
    default:
      std::cerr << "Unimplemented part type!" << std::endl;
      return;
  }

  buttons_arr_[qubit][layer] = button;
  *circuit_dirty_ = true;
}

void CircuitEditor::ReadFromCircuit() {
  /* This is really horrible and error-prone, I shouldn't really have to do this.... but alas... */
  // TODO: Do something different that doesn't involve this...
  for (Circuit::GridSize_T qubit = 0; qubit < Circuit::kMaxQubits; ++qubit) {
    for (Circuit::GridSize_T layer = 0; layer < Circuit::kMaxDepth; ++layer) {
      const Circuit::Part part_type = circuit_->GetPartTypeUnsafe(qubit, layer);
      const Circuit::Matrix_T *matrix = circuit_->GetMatrixUnsafe(qubit, layer);

      bool set = false;
      for (const GateButton *button : gate::kKnownGates) {
        if (part_type == button->part_type && button->MatrixMatches(matrix)) {
          buttons_arr_[qubit][layer] = button;
          set = true;
          break;
        }
      }

      if (!set) {
        std::cerr << "Unknown part!" << std::endl;

        if (matrix) {
          matrix->Print(std::cerr) << std::endl;
        }
      }
    }
  }
}

void CircuitPalette::Draw() {
  const ImGuiStyle &style = ImGui::GetStyle();
  const float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;

  constexpr std::size_t num_gates = gate::kKnownGates.size();
  for (int i = 0; std::cmp_less(i, num_gates); ++i) {
    ImGui::PushID(i);

    const GateButton *elem = gate::kKnownGates[i];
    ImGui::Button(elem->name, button_size_);

    const float last_button_x2 = ImGui::GetItemRectMax().x;
    const float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_size_.x;

    if ((i < num_gates - 1) && next_button_x2 < window_visible_x2) {
      ImGui::SameLine();
    }

    // DRAG AND DROP SOURCE
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
      ImGui::SetDragDropPayload("BUTTON_SET", &elem, sizeof(*elem));
      ImGui::Text("%s", "Set gate");
      ImGui::EndDragDropSource();
    }

    ImGui::PopID();
  }
}

void CircuitInfoPanel::Draw() {
  ImGui::Text("Circuit Info:");

  if (*circuit_dirty_ == true || info_str_.empty()) {
    RecomputeInfo();
  }

  ImGui::Text("%s", info_str_.data());
}

void CircuitInfoPanel::RecomputeInfo() {

  const auto parts = circuit_->GetPartsInLayer(0);
  const auto matrices = circuit_->GetMatricesInLayer(0);

  const Matrix2D<std::complex<float>> data = ComputeLayer(parts, matrices);

  std::ostringstream oss;

  data.Print(oss);

  info_str_ = oss.str();

  *circuit_dirty_ = false;
}
