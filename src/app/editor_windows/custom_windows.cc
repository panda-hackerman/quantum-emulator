//
// Created by Eli Michaud on 6/25/2026.
//

#include "custom_windows.h"

#include <bitset>
#include <iostream>
#include <utility>

#include "../application/application.h"
#include "imgui.h"
#include "implot.h"
#include "simulator/simulator.h"

/* --------------- CIRCUIT EDITOR ---------------*/

void CircuitEditor::Draw() {
  const TextureManager &texture_manager = Application::Instance().GetTextureManager();

  ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
  ImGui::InputInt("Qubits", &data.num_qubits);
  ImGui::SameLine();
  ImGui::InputInt("Layers", &data.num_layers);
  ImGui::PopItemWidth();

  // Update Size
  UpdateCircuitSize();

  ImGui::SameLine();
  if (ImGui::Button("Clear Circuit")) {
    ClearCircuit();
  }

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
    for (Circuit::GridSize_T qubit = 0; std::cmp_less(qubit, data.num_qubits); ++qubit) {

      { // Column 0 (labels)
        ImGui::PushID(qubit);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("q%d", qubit);
      }

      // Column 1 to N+1
      for (Circuit::GridSize_T layer = 0; std::cmp_less(layer, data.num_layers); ++layer) {
        ImGui::PushID(layer);
        ImGui::TableSetColumnIndex(layer + 1);

        const GateButton *gate_button = buttons_arr_[qubit][layer];
        const ImVec2 button_size = GetCircuitButtonSize();

        if (gate_button->sprite_id != SpriteID::kUndefined) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
          const Texture *texture = texture_manager.GetTexture(TextureID::kCircuit);
          const Sprite &sprite = kIdToSpriteMap.Get(gate_button->sprite_id);
          ImVec2 uv_1 = sprite.GetUV1(texture->Size());
          ImVec2 uv_2 = sprite.GetUV2(texture->Size());

          ImGui::ImageButton(gate_button->name, texture->GetViewRef(), button_size, uv_1, uv_2);
          ImGui::PopStyleVar();
          ImGui::PopStyleColor(3);
        } else {
          ImGui::Button(gate_button->name, button_size);
        }

        // DRAG AND DROP / SOURCE
        if (gate_button->part != Circuit::Part::kEmpty) {
          if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            GateSwapPayload payload = {qubit, layer};

            ImGui::SetDragDropPayload("BUTTON_SWAP", &payload, sizeof(payload));
            ImGui::Text("%s", "Move gate");
            ImGui::EndDragDropSource();
          }

          // Set mouse cursor
          if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
          }
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

void CircuitEditor::ClearCircuit() {
  circuit_->Clear();

  for (Circuit::GridSize_T qubit = 0; qubit < Circuit::kMaxQubits; ++qubit) {
    for (Circuit::GridSize_T layer = 0; layer < Circuit::kMaxDepth; ++layer) {
      buttons_arr_[qubit][layer] = &gate::kEmpty;
    }
  }

  *circuit_dirty_ = true;
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

  switch (button->part) {
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
        if (part_type == button->part && button->MatrixMatches(matrix)) {
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

/* --------------- CIRCUIT PALETTE ---------------*/

void CircuitPalette::Draw() {
  const ImGuiStyle &style = ImGui::GetStyle();
  const TextureManager &texture_manager = Application::Instance().GetTextureManager();

  const float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
  const ImVec2 button_size = GetCircuitButtonSize();

  constexpr std::size_t num_gates = gate::kKnownGates.size();
  for (int i = 0; std::cmp_less(i, num_gates); ++i) {
    ImGui::PushID(i);

    const GateButton *gate_button = gate::kKnownGates[i];
    // ImGui::Button(gate_button->name, button_size);

    if (gate_button->sprite_id != SpriteID::kUndefined) {
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
      const Texture *texture = texture_manager.GetTexture(TextureID::kCircuit);
      const Sprite &sprite = kIdToSpriteMap.Get(gate_button->sprite_id);
      ImVec2 uv_1 = sprite.GetUV1(texture->Size());
      ImVec2 uv_2 = sprite.GetUV2(texture->Size());

      ImGui::ImageButton(gate_button->name, texture->GetViewRef(), button_size, uv_1, uv_2);
      ImGui::PopStyleVar();
      ImGui::PopStyleColor(3);
    } else {
      ImGui::Button(gate_button->name, button_size);
    }

    const float last_button_x2 = ImGui::GetItemRectMax().x;
    const float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_size.x;

    if ((i < num_gates - 1) && next_button_x2 < window_visible_x2) {
      ImGui::SameLine();
    }

    // DRAG AND DROP SOURCE
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
      ImGui::SetDragDropPayload("BUTTON_SET", &gate_button, sizeof(*gate_button));
      ImGui::Text("%s", "Set gate");
      ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemHovered()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImGui::PopID();
  }
}

/* --------------- CIRCUIT INFO PANEL ---------------*/

void CircuitInfoPanel::Draw() {
  ImGui::Checkbox("Skip outputs with 0 probability", &data.skip_empty_probs);

  if (*circuit_dirty_ == true || info_str_.empty()) {
    RecomputeInfo();
  }

  if (ImPlot::BeginPlot("Probability Distribution:", {-1, 0},
                        ImPlotFlags_Equal | ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {

    std::vector<const char *> x_axis_labels;
    std::vector<double> probabilities;

    std::vector<double> positions;
    double next_pos = 0;

    for (int i = 0; i < plot_labels_.size(); ++i) {
      if (data.skip_empty_probs && plot_probs_[i] == 0) continue;

      x_axis_labels.emplace_back(plot_labels_[i].data()); // Ignore error, compiles fine :P
      probabilities.push_back(plot_probs_[i]);

      positions.push_back(next_pos++);
    }

    const int groups_size = static_cast<int>(positions.size());

    ImPlot::SetupAxes("Output", "Probability", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1, ImGuiCond_Always);
    ImPlot::SetupAxisTicks(ImAxis_X1, positions.data(), groups_size, x_axis_labels.data());
    ImPlot::PlotBars("", probabilities.data(), groups_size, 0.8, 0);

    ImPlot::EndPlot();
  }

  ImGui::Text("%s", info_str_.data());
}

void CircuitInfoPanel::RecomputeInfo() {

  state_vector_ = SimulateCircuitQubitWise(*circuit_);

  const Circuit::GridSize_T num_qubits = circuit_->GetNumQubits();
  const int state_vector_size = 1 << num_qubits;

  if (state_vector_.size() != state_vector_size) {
    std::cerr << "Invalid state vector size; must be 2^n for n qubits." << std::endl;
    return;
  }

  plot_labels_.clear();
  plot_probs_.clear();

  for (int i = state_vector_size - 1; i >= 0; --i) {
    const Complex &complex = state_vector_[i];
    const double probability = std::pow(std::abs(complex), 2);

    // Convert decimal -> binary
    std::string bit_string = Bitset(i).to_string();
    bit_string = bit_string.substr(bit_string.size() - num_qubits);
    std::ranges::reverse(bit_string);

    plot_labels_.push_back(bit_string);
    plot_probs_.push_back(probability);
  }

  *circuit_dirty_ = false;
}

/* --------------- MISC ---------------*/

ImVec2 GetCircuitButtonSize() {
  constexpr ImVec2 norm = {80, 80};
  const float factor = ImGui::GetFontSize() / 13;

  return {norm.x * factor, norm.y * factor};
}
