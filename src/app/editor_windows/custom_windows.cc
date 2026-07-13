//
// Created by Eli Michaud on 6/25/2026.
//

#include "custom_windows.h"

#include <bitset>
#include <iostream>
#include <numeric>
#include <utility>

#include "../application/application.h"
#include "../theme.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "simulator/simulator.h"

/* --------------- MISC ---------------*/

/**
 * Scale a Vec2D by the current DPI.
 * @param vec2 The size to scale
 * @return A new ImVec2
 */
ImVec2 ScaleDPI(const ImVec2 vec2) {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  const float dpi = viewport->DpiScale;

  return {vec2.x * dpi, vec2.y * dpi};
}

/**
 * Scale a float by the current DPI.
 * @param x The float to scale
 * @return A new (scaled) float
 */
float ScaleDPI(const float x) {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  const float dpi = viewport->DpiScale;

  return x * dpi;
}

/* --------------- CIRCUIT EDITOR ---------------*/

void CircuitEditor::Draw() {
  const TextureManager &texture_manager = Application::Instance().GetTextureManager();
  const ImGuiStyle *style = &ImGui::GetStyle();

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

  const ImVec2 button_size = ScaleDPI(theme::kCircuitButtonDefaultSize);

  // Actually draw
  if (ImGui::BeginTable("Circuit Diagram", data.num_layers + 1, table_flags)) {

    const ImGuiTable *table = ImGui::GetCurrentTable();

    // Setup Columns
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize);

    for (int i = 0; i < data.num_layers; ++i) {
      ImGui::TableSetupColumn(std::format("t{}", i).data(),
                              ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize);
    }

    ImGui::TableHeadersRow();

    // Loop over actual grid
    for (Circuit::GridSize_T qubit = 0; std::cmp_less(qubit, data.num_qubits); ++qubit) {

      ImGui::PushID(qubit);

      /* [COLUMN 0 - LABELS] */ {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("q%d", qubit);
      }

      /* [DRAW HORIZONTAL LINES] */ {
        const ImGuiTableColumn *column = &table->Columns[0];
        const float min_x = table->OuterRect.Min.x; /* Left of table  */
        const float max_x = table->OuterRect.Max.x; /* Right of table */
        const float top_y = table->OuterRect.Max.y; /* Top of table   */
        const float col_0_w = column->WidthGiven; /* Width of column 0 */ // FIXME: Fragile?
        const float header_height = !table->IsUsingHeaders ? 0 : ImGui::TableGetHeaderRowHeight();
        const float button_height = button_size.y + (style->CellPadding.y * 2); /* Incl. padding */

        const float start_y = top_y + header_height + (button_size.y / 2) + style->CellPadding.y;
        const float start_x = min_x + col_0_w + (style->CellPadding.x * 2) + 1.0f;

        const float y_pos = start_y + (qubit * button_height);
        const float thickness = ScaleDPI(theme::kCircuitLineWidthH);

        ImGui::GetWindowDrawList()->AddLineH(start_x, max_x, y_pos, theme::kBlack800Color,
                                             thickness);
      }

      // Column 1 to N+1
      for (Circuit::GridSize_T layer = 0; std::cmp_less(layer, data.num_layers); ++layer) {
        ImGui::PushID(layer);
        ImGui::TableSetColumnIndex(layer + 1);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        const GateButton *gate_button = buttons_arr_[qubit][layer];

        //[DRAW VERTICAL LINES]
        if (gate_button->part != Circuit::Part::kEmpty &&
            gate_button->part != Circuit::Part::kMeasure) {
          const ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
          const float x_pos = cursor_pos.x + (button_size.x / 2);
          const float y_pos = cursor_pos.y + (button_size.y / 2);
          const float button_height = button_size.y + (style->CellPadding.y * 2); /* Incl. padding*/

          for (Circuit::GridSize_T q = qubit + 1; std::cmp_less(q, data.num_qubits); ++q) {
            const GateButton *other = buttons_arr_[q][layer];

            if (!ShouldConnectGates(gate_button->part, other->part, layer)) continue;

            const float num_down = static_cast<float>(q - qubit); /* How many qubits down are we? */
            const float other_y_pos = y_pos + (button_height * num_down);
            const float thickness = ScaleDPI(theme::kCircuitLineWidthV);

            ImGui::GetWindowDrawList()->AddLineV(x_pos, y_pos, other_y_pos,
                                                 theme::kCircuitLineColorV, thickness);
            break; // Only draw one line
          }
        }

        // Make button transparent
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(theme::kTransparentColor));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(theme::kTransparentColor));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(theme::kTransparentColor));

        if (gate_button->sprite_id != SpriteID::kUndefined) {
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
          const Texture *texture = texture_manager.GetTexture(TextureID::kCircuit);
          const Sprite &sprite = kIdToSpriteMap.Get(gate_button->sprite_id);
          ImVec2 uv_1 = sprite.GetUV1(texture->Size());
          ImVec2 uv_2 = sprite.GetUV2(texture->Size());

          ImGui::ImageButton(gate_button->name, texture->GetViewRef(), button_size, uv_1, uv_2);
          ImGui::PopStyleVar();
        } else {
          ImGui::Button(gate_button->name, button_size);
        }

        ImGui::PopStyleColor(3); // ImGuiCol_Button[Hovered/Active] ...

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
          constexpr ImGuiDragDropFlags flags =
              ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip;

          if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(kPayloadTypeSwap, flags)) {
            auto [payload_qubit, payload_layer] = *static_cast<GateSwapPayload *>(payload->Data);
            const bool valid = IsValidSwap(qubit, layer, payload_qubit, payload_layer);

            if (valid && payload->IsDelivery()) {
              Set(qubit, layer, buttons_arr_[payload_qubit][payload_layer]);
              Set(payload_qubit, payload_layer, gate_button);
            }

            // Mouse/ tooltips
            if (!valid) {
              ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
              ImGui::SetTooltip("%s", "Cannot place here!");
            } else {
              ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
          }

          if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(kPayloadTypeSet, flags)) {
            const GateButton *payload_button = *static_cast<const GateButton **>(payload->Data);
            const bool valid = IsValidSet(qubit, layer, payload_button->part);

            if (valid && payload->IsDelivery()) {
              Set(qubit, layer, payload_button);
            }

            if (!valid) {
              ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
              ImGui::SetTooltip("%s", "Cannot place here!");
            } else {
              ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
          }

          ImGui::EndDragDropTarget();
        }

        ImGui::PopStyleVar(); // ImGuiStyleVar_FrameBorderSize
        ImGui::PopID(); // layer
      }

      ImGui::PopID(); // qubit
    }

    ImGui::EndTable(); // Circuit Diagram
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

bool CircuitEditor::ShouldConnectGates(const Circuit::Part a, const Circuit::Part b,
                                       const Circuit::GridSize_T layer) const {

  using enum Circuit::Part;

  if (a == kEmpty || b == kEmpty) return false;
  if (a == kMeasure || b == kMeasure) return false;

  const bool exists_valid_swap = circuit_->ExistsValidSwapInLayer(layer);
  const bool exists_matrix = circuit_->ExistsInLayer(kMatrix2x2, layer);

  switch (a) {
    case kMatrix2x2:
      return Circuit::IsControlBit(b);
    case kControlBit:
    case kAntiControlBit:
      switch (b) { /* Control bit */
        case kMatrix2x2:
          return true;
        case kControlBit:
        case kAntiControlBit:
          return exists_valid_swap || exists_matrix;
        case kSwap:
          return exists_valid_swap;
        default:
          return false;
      }
    case kSwap:
      switch (b) { /* Swap gate */
        case kMatrix2x2:
          return false;
        case kControlBit:
        case kAntiControlBit:
          return exists_valid_swap;
        case kSwap:
          return true;
        default:
          return false;
      }
    default:
      return false;
  }
}

bool CircuitEditor::IsValidSwap(const Circuit::GridSize_T qubit_a,
                                const Circuit::GridSize_T layer_a,
                                const Circuit::GridSize_T qubit_b,
                                const Circuit::GridSize_T layer_b) const {
  if (layer_a == layer_b) {
    return true; // There aren't any rules depending on layer position
  }

  std::vector<Circuit::Part> parts_a = circuit_->GetPartsInLayer(layer_a);
  std::vector<Circuit::Part> parts_b = circuit_->GetPartsInLayer(layer_b);

  parts_a.at(qubit_a) = buttons_arr_[qubit_b][layer_b]->part;
  parts_b.at(qubit_b) = buttons_arr_[qubit_a][layer_a]->part;

  return Circuit::IsValidLayer(parts_a) && Circuit::IsValidLayer(parts_b);
}

bool CircuitEditor::IsValidSet(const Circuit::GridSize_T qubit, const Circuit::GridSize_T layer,
                               const Circuit::Part part) const {

  std::vector<Circuit::Part> parts = circuit_->GetPartsInLayer(layer);
  parts.at(qubit) = part;

  return Circuit::IsValidLayer(parts);
}

/* --------------- CIRCUIT PALETTE ---------------*/

void CircuitPalette::Draw() {
  const ImGuiStyle &style = ImGui::GetStyle();
  const TextureManager &texture_manager = Application::Instance().GetTextureManager();

  const float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
  const ImVec2 button_size = ScaleDPI(theme::kCircuitButtonDefaultSize);

  constexpr std::size_t num_gates = gate::kKnownGates.size();
  for (int i = 0; std::cmp_less(i, num_gates); ++i) {
    ImGui::PushID(i);

    const GateButton *gate_button = gate::kKnownGates[i];

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

    // Set hover cursor/ tooltip
    if (ImGui::IsItemHovered()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      ImGui::SetTooltip("%s", gate_button->name);
    }

    ImGui::PopID();
  }
}

/* --------------- CIRCUIT INFO PANEL ---------------*/
void CircuitInfoPanel::Draw() {
  ImGui::Checkbox("Skip outputs with 0 probability", &data.skip_empty_probs);

  if (*circuit_dirty_ == true) {
    RecomputeInfo(); // TODO: Could be moved
  }

  if (ImPlot::BeginPlot("Probability Distribution:", {-1, 0},
                        ImPlotFlags_Equal | ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {

    const std::vector<const char *> *x_axis_labels;
    const std::vector<double> *probabilities;

    if (!data.skip_empty_probs) {
      x_axis_labels = &info_.labels_c;
      probabilities = &info_.densities;
    } else {
      x_axis_labels = &info_.labels_c_nonzero;
      probabilities = &info_.densities_nonzero;
    }

    const int count = static_cast<int>(x_axis_labels->size());

    std::vector<double> positions(count);
    std::ranges::iota(positions, 0);

    ImPlot::SetupAxes("Output", "Probability", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1, ImGuiCond_Always);
    ImPlot::SetupAxisTicks(ImAxis_X1, positions.data(), count, x_axis_labels->data());
    ImPlot::PlotBars("", probabilities->data(), count, kBarSize, 0);

    ImPlot::EndPlot();
  }
}

void CircuitInfoPanel::RecomputeInfo() {
  state_vector_ = StateVector{circuit_->GetNumQubits()};
  ApplyCircuitQubitWise(*circuit_, state_vector_);

  info_.RecalculateData(state_vector_);

  *circuit_dirty_ = false;
}