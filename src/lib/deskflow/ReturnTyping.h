/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include "deskflow/KeyTypes.h"

#include <array>
#include <cstddef>

inline constexpr uint32_t kReturnTypingText = 1u << 0;
inline constexpr uint32_t kReturnTypingClipboardHistory = 1u << 1;
inline constexpr uint32_t kReturnTypingShortcuts = 1u << 2;

inline constexpr KeyID kReturnTypingClipboardHistoryCode = 1;
inline constexpr KeyID kReturnTypingShortcutCodeBase = 0xf100;

struct ReturnTypingShortcut
{
  KeyID macKey;
  KeyModifierMask macModifiers;
  KeyID windowsKey;
  KeyModifierMask windowsModifiers;

  constexpr bool acceptsRepeat() const
  {
    return macModifiers == 0 && (macKey == kKeyLeft || macKey == kKeyRight || macKey == kKeyUp || macKey == kKeyDown);
  }
};

// The first row keeps the original clipboard history code. Append new rows to preserve the wire codes.
inline constexpr auto kReturnTypingShortcutMap = std::to_array<ReturnTypingShortcut>({
    {'v', KeyModifierSuper, 'v', KeyModifierSuper},
    {'v', KeyModifierControl, 'v', KeyModifierControl},
    {'v', KeyModifierSuper | KeyModifierShift, 'v', KeyModifierControl | KeyModifierShift},
    {'c', KeyModifierSuper, 'c', KeyModifierControl},
    {'x', KeyModifierSuper, 'x', KeyModifierControl},
    {'a', KeyModifierSuper, 'a', KeyModifierControl},
    {'z', KeyModifierSuper, 'z', KeyModifierControl},
    {'z', KeyModifierSuper | KeyModifierShift, 'y', KeyModifierControl},
    {'s', KeyModifierSuper, 's', KeyModifierControl},
    {'s', KeyModifierSuper | KeyModifierShift, 's', KeyModifierControl | KeyModifierShift},
    {'f', KeyModifierSuper, 'f', KeyModifierControl},
    {'g', KeyModifierSuper, 'g', KeyModifierControl},
    {'p', KeyModifierSuper, 'p', KeyModifierControl},
    {'o', KeyModifierSuper, 'o', KeyModifierControl},
    {'n', KeyModifierSuper, 'n', KeyModifierControl},
    {'n', KeyModifierSuper | KeyModifierShift, 'n', KeyModifierControl | KeyModifierShift},
    {'t', KeyModifierSuper, 't', KeyModifierControl},
    {'t', KeyModifierSuper | KeyModifierShift, 't', KeyModifierControl | KeyModifierShift},
    {'w', KeyModifierSuper, 'w', KeyModifierControl},
    {'l', KeyModifierSuper, 'l', KeyModifierControl},
    {'r', KeyModifierSuper, 'r', KeyModifierControl},
    {'=', KeyModifierSuper, '=', KeyModifierControl},
    {'-', KeyModifierSuper, '-', KeyModifierControl},
    {'0', KeyModifierSuper, '0', KeyModifierControl},
    {kKeyTab, KeyModifierAlt, kKeyTab, KeyModifierAlt},
    {kKeyTab, KeyModifierAlt | KeyModifierShift, kKeyLeftTab, KeyModifierAlt | KeyModifierShift},
    {kKeyTab, KeyModifierSuper | KeyModifierAlt, kKeyTab, KeyModifierSuper},
    {'e', KeyModifierSuper | KeyModifierAlt, 'e', KeyModifierSuper},
    {'d', KeyModifierSuper | KeyModifierAlt, 'd', KeyModifierSuper},
    {'s', KeyModifierSuper | KeyModifierAlt, 's', KeyModifierSuper},
    {'i', KeyModifierSuper | KeyModifierAlt, 'i', KeyModifierSuper},
    {'r', KeyModifierSuper | KeyModifierAlt, 'r', KeyModifierSuper},
    {'w', KeyModifierSuper | KeyModifierAlt, kKeyF4, KeyModifierAlt},
    {'s', KeyModifierSuper | KeyModifierAlt | KeyModifierShift, 's', KeyModifierSuper | KeyModifierShift},
    {kKeyTab, 0, kKeyTab, 0},
    {kKeyTab, KeyModifierShift, kKeyLeftTab, KeyModifierShift},
    {kKeyEscape, 0, kKeyEscape, 0},
    {kKeyLeft, 0, kKeyLeft, 0},
    {kKeyRight, 0, kKeyRight, 0},
    {kKeyUp, 0, kKeyUp, 0},
    {kKeyDown, 0, kKeyDown, 0},
    {kKeyHome, 0, kKeyHome, 0},
    {kKeyEnd, 0, kKeyEnd, 0},
    {kKeyPageUp, 0, kKeyPageUp, 0},
    {kKeyPageDown, 0, kKeyPageDown, 0},
});

constexpr const ReturnTypingShortcut *returnTypingShortcut(KeyID code, uint32_t options)
{
  if (code == kReturnTypingClipboardHistoryCode && (options & kReturnTypingClipboardHistory) != 0) {
    return &kReturnTypingShortcutMap.front();
  }
  if ((options & kReturnTypingShortcuts) != 0 && code >= kReturnTypingShortcutCodeBase &&
      code - kReturnTypingShortcutCodeBase < kReturnTypingShortcutMap.size() - 1) {
    return &kReturnTypingShortcutMap[code - kReturnTypingShortcutCodeBase + 1];
  }
  return nullptr;
}

constexpr KeyID returnTypingShortcutCode(KeyID key, KeyModifierMask modifiers, uint32_t options)
{
  for (std::size_t i = 0; i < kReturnTypingShortcutMap.size(); ++i) {
    const auto &shortcut = kReturnTypingShortcutMap[i];
    if (shortcut.macKey == key && shortcut.macModifiers == modifiers) {
      const auto code = i == 0 ? kReturnTypingClipboardHistoryCode : kReturnTypingShortcutCodeBase + KeyID(i - 1);
      return returnTypingShortcut(code, options) ? code : kKeyNone;
    }
  }
  return kKeyNone;
}

constexpr bool returnTypingCodeOffered(KeyID code, uint32_t options)
{
  return ((options & kReturnTypingText) != 0 && isReturnTypingKey(code)) ||
         returnTypingShortcut(code, options) != nullptr;
}
