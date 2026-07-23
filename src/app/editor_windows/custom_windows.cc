//
// Created by Eli Michaud on 6/25/2026.
//

#include "custom_windows.h"

#include <numeric>
#include <utility>

#include "../application/application.h"
#include "../theme.h"
#include "editor_window_manager.h"
#include "imgui_internal.h"
#include "implot.h"

/* --------------- UTIL --------------- */
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

namespace editor::circuit {
/* --------------- EDITOR WINDOWS --------------- */
EditorWindow main_window = {
    .name = "Circuit Diagram Editor",
    .on_draw = [] { DrawEditor(); },
    .flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar,
    .can_close = false,
};

EditorWindow info_window = {
    .name = "Circuit Info",
    .on_draw = [] { DrawInfo(); },
    .can_close = false,
};

EditorWindow palette_window = {
    .name = "Circuits",
    .on_draw = [] { DrawPalette(); },
    .can_close = false,
};

/* --------------- CIRCUIT EDITOR --------------- */

void DrawEditor() {
  const TextureManager &texture_manager = Application::Instance().GetTextureManager();
  const ImGuiStyle *style = &ImGui::GetStyle();

  ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
  ImGui::InputInt("Qubits", &gui_data.num_qubits);
  ImGui::SameLine();
  ImGui::InputInt("Layers", &gui_data.num_layers);
  ImGui::PopItemWidth();

  UpdateCircuitSize();
  const GridSize num_qubits = static_cast<GridSize>(gui_data.num_qubits);
  const GridSize num_layers = static_cast<GridSize>(gui_data.num_layers);

  ImGui::Checkbox("Qubit Info", &gui_data.show_qubit_info);

  ImGui::SameLine();
  if (ImGui::Button("Clear")) {
    ClearEditor();
  }

  // Table
  const ImVec2 button_size = ScaleDPI(theme::kCircuitButtonDefaultSize);

  /* We have two child windows
   * - Inner child will grow or shrink to fit the size of the table.
   * - Outer child will grow or shrink to fit the size of the inner child, but will never exceed the
   *    size of the parent window. Since it has scrolling enabled, this enables scrolling for the
   *    inner child (thus the table) via this window.
   * This seems super unnecessary, but ImGui table stuff is fucked and I can't come up with a
   * better solution that doesn't force the table to grow to the size of the parent window (which
   * looks really weird).
   */
  ImGui::SetNextWindowSizeConstraints({0.0f, 0.0f}, ImGui::GetContentRegionAvail());
  ImGui::BeginChild(
      "Outer Child", {0.0f, 0.0f},
      ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_NavFlattened,
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HorizontalScrollbar);
  ImGui::BeginChild(
      "Inner Child", {0.0f, 0.0f},
      ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_NavFlattened,
      ImGuiWindowFlags_NoSavedSettings);

  constexpr ImGuiTableFlags table_flags =
      ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody;
  constexpr ImGuiTableColumnFlags col_flags =
      ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize;

  // Qubit label column + layers + qubit info
  const int num_table_cols = num_layers + 1 + (gui_data.show_qubit_info ? 1 : 0);

  if (ImGui::BeginTable("Circuit Diagram", num_table_cols, table_flags)) {

    // ImGui::TableSetupScrollFreeze(1, 1); // FIXME: Scroll freeze doesn't work

    // Setup Columns
    ImGui::TableSetupColumn("Qubit Num.", col_flags | ImGuiTableColumnFlags_NoHeaderLabel);

    for (int i = 0; std::cmp_less(i , num_layers); ++i) {
      ImGui::TableSetupColumn(std::format("t{}", i).data(), col_flags);
    }

    ImGui::TableHeadersRow();

    // Loop over actual grid
    for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {

      ImGui::PushID(qubit);
      ImGui::TableNextRow();

      /* [COLUMN 0 - LABELS] */ {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("q%d", qubit);
      }

      // [COLUMN 1 -> N+1 - Qubits]
      for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {
        ImGui::PushID(layer);
        ImGui::TableSetColumnIndex(layer + 1);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        const GateButton *gate_button = editor_data.buttons_arr[qubit][layer];

        /* [DRAW HORIZONTAL/ VERTICAL LINES]*/ {
          const ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
          const float x_mid_pos = cursor_pos.x + (button_size.x / 2);
          const float y_mid_pos = cursor_pos.y + (button_size.y / 2);

          const float thickness_h = ScaleDPI(theme::kCircuitLineWidthH);
          const float thickness_v = ScaleDPI(theme::kCircuitLineWidthV);

          // Size including padding
          const float button_height = button_size.y + (style->CellPadding.y * 2);
          const float button_width = button_size.x + (style->CellPadding.x * 2);

          // Horizontal lines
          const float x_left = x_mid_pos - (button_width / 2) - 1;
          const float x_right = x_mid_pos + (button_width / 2);

          ImGui::GetWindowDrawList()->AddLineH(x_left, x_right, y_mid_pos,
                                               theme::kCircuitLineColorH, thickness_h);

          // Vertical lines - (for each qubit below)
          for (Circuit::GridSize_T q = qubit + 1; std::cmp_less(q, num_qubits); ++q) {

            const GateButton *other = editor_data.buttons_arr[q][layer];
            if (!ShouldConnectGates(gate_button->part, other->part, layer)) continue;

            const float num_down = static_cast<float>(q - qubit); /* How many qubits down are we? */
            const float other_y_pos = y_mid_pos + (button_height * num_down);

            ImGui::GetWindowDrawList()->AddLineV(x_mid_pos, y_mid_pos, other_y_pos,
                                                 theme::kCircuitLineColorV, thickness_v);
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
        if (gate_button->part != Part::kEmpty) {
          if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            GateSwapPayload payload = {qubit, layer};

            ImGui::SetDragDropPayload(kPayloadTypeSwap, &payload, sizeof(payload));
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
              SetButton(qubit, layer, editor_data.buttons_arr[payload_qubit][payload_layer]);
              SetButton(payload_qubit, payload_layer, gate_button);
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
              SetButton(qubit, layer, payload_button);
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

      // [COLUMN N+2 - Qubit Info]
      if (gui_data.show_qubit_info) {
        ImGui::TableSetColumnIndex(num_table_cols - 1);
        ImGui::Text("Info:");
      }

      ImGui::PopID(); // qubit
    }

    ImGui::EndTable(); // Circuit Diagram

    /* Draw border for first (qubit) column */ {
      // TODO: Draw for qubit info also
      ImGuiContext *ctx = ImGui::GetCurrentContext();
      ImGuiTable *table = ctx->Tables.GetByKey(ImGui::GetID("Circuit Diagram"));
      ImGuiTableColumn *first_col = &table->Columns[0];
      const float header_height = table->IsUsingHeaders ? ImGui::TableGetHeaderRowHeight(table) : 0;

      const auto max_x = first_col->MaxX;
      const auto min_y = table->InnerRect.Min.y;
      const auto max_y = table->InnerRect.Max.y;
      const auto border_color = table->BorderColorLight;
      table->InnerWindow->DrawList->AddLineV(max_x, (min_y + header_height), max_y, border_color,
                                             1);
    }
  }

  ImGui::EndChild(); // Inner Child
  ImGui::EndChild(); // Outer Child
}

void ClearEditor() {
  Circuit &circuit = Application::Instance().circuit;
  circuit.Clear();

  for (Circuit::GridSize_T qubit = 0; qubit < Circuit::kMaxQubits; ++qubit) {
    for (Circuit::GridSize_T layer = 0; layer < Circuit::kMaxDepth; ++layer) {
      editor_data.buttons_arr[qubit][layer] = &gate::kEmpty;
    }
  }

  editor_data.circuit_dirty = true;
}

void UpdateCircuitSize() {
  Circuit &circuit = Application::Instance().circuit;

  gui_data.num_qubits =
      std::clamp<int>(gui_data.num_qubits, Circuit::kMinQubits, Circuit::kMaxQubits);
  gui_data.num_layers =
      std::clamp<int>(gui_data.num_layers, Circuit::kMinDepth, Circuit::kMaxDepth);

  const GridSize qubits = static_cast<GridSize>(gui_data.num_qubits);
  const GridSize layers = static_cast<GridSize>(gui_data.num_layers);

  if (qubits != circuit.GetNumQubits() || layers != circuit.GetNumLayers()) {
    circuit.SetSize(qubits, layers);
    editor_data.circuit_dirty = true;
  }
}

/* --------------- CIRCUIT INFO PANEL --------------- */

void DrawInfo() {
  ImGui::Checkbox("Skip outputs with 0 probability", &gui_data.skip_zero_probabilities);

  const bool skip_zeroes = gui_data.skip_zero_probabilities;
  const auto &info = Application::Instance().current_circuit_info;

  if (ImPlot::BeginPlot("Probability Distribution:", {-1, 0},
                        ImPlotFlags_Equal | ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {

    const std::vector<const char *> *x_axis_labels;
    const std::vector<double> *probabilities;

    if (!skip_zeroes) {
      x_axis_labels = &info.labels_c;
      probabilities = &info.densities;
    } else {
      x_axis_labels = &info.labels_c_nonzero;
      probabilities = &info.densities_nonzero;
    }

    const int count = static_cast<int>(x_axis_labels->size());

    std::vector<double> positions(count);
    std::ranges::iota(positions, 0);

    ImPlot::SetupAxes("Output", "Probability Density", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1, ImGuiCond_Always);
    ImPlot::SetupAxisTicks(ImAxis_X1, positions.data(), count, x_axis_labels->data());
    ImPlot::PlotBars("", probabilities->data(), count, kInfoGraphBarSize, 0);

    ImPlot::EndPlot();
  }
}

/* --------------- CIRCUIT PALETTE --------------- */

void DrawPalette() {
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
      ImGui::SetDragDropPayload(kPayloadTypeSet, &gate_button, sizeof(*gate_button));
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

/* --------------- GENERAL --------------- */

void SetExampleCircuit() {
  gui_data.num_qubits = 3;
  gui_data.num_layers = 4;
  UpdateCircuitSize();

  SetButton(1, 0, &gate::kHadamard);
  SetButton(2, 0, &gate::kPauliX);

  SetButton(0, 1, &gate::kPauliX);
  SetButton(1, 1, &gate::kControlBit);

  SetButton(0, 2, &gate::kPauliZ);

  SetButton(1, 3, &gate::kControlBit);
  SetButton(2, 3, &gate::kPauliX);
}

void SetButton(const GridSize qubit, const GridSize layer, const GateButton *button) {
  using enum Circuit::Part;
  Circuit &circuit = Application::Instance().circuit;

  if (std::cmp_greater(qubit, gui_data.num_qubits) || std::cmp_greater(layer, gui_data.num_layers))
    return;

  switch (button->part) {
    case kEmpty:
      circuit.AddEmpty(qubit, layer);
      break;
    case kMatrix2x2:
      circuit.AddGate(qubit, layer, button->matrix);
      break;
    case kControlBit:
      circuit.AddControlBit(qubit, layer);
      break;
    case kAntiControlBit:
      circuit.AddAntiControlBit(qubit, layer);
      break;
    case kMeasure:
      circuit.AddMeasurement(qubit, layer);
      break;
    case kSwap:
      circuit.AddSwap(qubit, layer);
      break;
    default:
      std::cerr << "Unimplemented part type!" << std::endl;
      return;
  }

  editor_data.buttons_arr[qubit][layer] = button;
  editor_data.circuit_dirty = true;
}

bool ShouldConnectGates(const Part a, const Part b, const GridSize layer) {
  using enum Circuit::Part;

  if (a == kEmpty || b == kEmpty) return false;
  if (a == kMeasure || b == kMeasure) return false;

  if (std::cmp_less(gui_data.num_qubits, Circuit::kMinQubits) ||
      std::cmp_less(gui_data.num_layers, Circuit::kMinDepth)) {
    return false; // Not initialized
  }

  int num_swaps = 0;
  bool exists_matrix = false;
  bool exists_control = false;

  for (int qubit = 0; qubit < gui_data.num_qubits; ++qubit) {
    const GateButton *button = editor_data.buttons_arr[qubit][layer];

    if (button->part == kSwap) num_swaps++;
    if (button->part == kMatrix2x2) exists_matrix = true;
    if (button->part == kControlBit || button->part == kAntiControlBit) exists_control = true;
  }

  const bool exists_valid_swap = num_swaps >= 2;

  switch (a) {
    case kMatrix2x2:
      switch (b) /* Matrix */ {
        case kMatrix2x2:
          return exists_control;
        case kControlBit:
        case kAntiControlBit:
          return true;
        default:
          return false;
      }
    case kControlBit:
    case kAntiControlBit:
      switch (b) /* Control */ {
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
      switch (b) /* Swap */ {
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

bool IsValidSet(const GridSize qubit, const GridSize layer, const Part part) {
  const Circuit &circuit = Application::Instance().circuit;

  // TODO: Unnecessarily heavy ?
  std::vector<Circuit::Part> parts =
      circuit.GetPartsInLayer(layer) | std::ranges::to<decltype(parts)>();
  parts.at(qubit) = part;

  return Circuit::IsValidLayer(parts);
}

bool IsValidSwap(const GridSize qubit_a, const GridSize layer_a, const GridSize qubit_b,
                 const GridSize layer_b) {
  const Circuit &circuit = Application::Instance().circuit;
  const Part a_part = editor_data.buttons_arr[qubit_a][layer_a]->part;
  const Part b_part = editor_data.buttons_arr[qubit_b][layer_b]->part;

  if (layer_a == layer_b) return true;
  if (a_part == b_part) return true;

  if (a_part == Part::kEmpty) {
    return IsValidSet(qubit_a, layer_a, b_part);
  }

  if (b_part == Part::kEmpty) {
    return IsValidSet(qubit_b, layer_b, a_part);
  }

  std::vector<Circuit::Part> parts_a =
      circuit.GetPartsInLayer(layer_a) | std::ranges::to<decltype(parts_a)>();
  std::vector<Circuit::Part> parts_b =
      circuit.GetPartsInLayer(layer_b) | std::ranges::to<decltype(parts_b)>();

  parts_a.at(qubit_a) = b_part;
  parts_b.at(qubit_b) = a_part;

  return Circuit::IsValidLayer(parts_a) && Circuit::IsValidLayer(parts_b);
}

} // namespace editor::circuit