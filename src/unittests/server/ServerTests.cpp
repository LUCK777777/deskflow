/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Chris Rizzitello <sithlord48@gmail.com>
 * SPDX-FileCopyrightText: (C) 2014 - 2016 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "ServerTests.h"

#include "deskflow/KeyTypes.h"
#include "deskflow/ReturnTyping.h"
#include "server/Server.h"

void ServerTests::SwitchToComputerInfo_alloc_computer()
{
  auto actual = new Server::SwitchToComputerInfo("test");
  QCOMPARE(actual->m_computer, "test");
  delete actual;
}

void ServerTests::KeyboardBroadcastInfo_alloc_stateAndComputers()
{
  auto info = new Server::KeyboardBroadcastInfo(Server::KeyboardBroadcastInfo::State::kOn, "test");
  QCOMPARE(info->m_state, Server::KeyboardBroadcastInfo::State::kOn);
  QCOMPARE(info->m_computers, "test");
  delete info;
}

void ServerTests::returnTypingAcceptsOnlyTextKeys()
{
  QVERIFY(isReturnTypingKey(' '));
  QVERIFY(isReturnTypingKey('A'));
  QVERIFY(isReturnTypingKey('~'));
  QVERIFY(isReturnTypingKey(kKeyBackSpace));
  QVERIFY(isReturnTypingKey(kKeyReturn));
  QVERIFY(!isReturnTypingKey(kKeyNone));
  QVERIFY(!isReturnTypingKey(kKeyDelete));
  QVERIFY(!isReturnTypingKey(kKeyControl_L));
  QVERIFY(!isReturnTypingKey(0x4E2D));
}

void ServerTests::returnTypingClipboardHistoryRequiresAnOffer()
{
  constexpr auto clipboardHistory = kReturnTypingClipboardHistoryCode;
  QVERIFY(!returnTypingCodeOffered(clipboardHistory, 0));
  QVERIFY(!returnTypingCodeOffered(clipboardHistory, kReturnTypingText));
  QVERIFY(returnTypingCodeOffered(clipboardHistory, kReturnTypingText | kReturnTypingClipboardHistory));
  QVERIFY(returnTypingCodeOffered('v', kReturnTypingText));
  QVERIFY(!returnTypingCodeOffered(kKeySuper_L, kReturnTypingText | kReturnTypingClipboardHistory));
}

void ServerTests::returnTypingShortcutsMatchExactChords()
{
  constexpr auto previous = kReturnTypingText | kReturnTypingClipboardHistory;
  constexpr auto current = previous | kReturnTypingShortcuts;
  QCOMPARE(returnTypingShortcutCode('v', KeyModifierSuper, previous), kReturnTypingClipboardHistoryCode);
  QCOMPARE(returnTypingShortcutCode('c', KeyModifierSuper, previous), kKeyNone);
  QCOMPARE(returnTypingShortcutCode(kKeyTab, KeyModifierSuper, current), kKeyNone);
  QCOMPARE(returnTypingShortcutCode(kKeyTab, KeyModifierAlt | KeyModifierControl, current), kKeyNone);
  QCOMPARE(returnTypingShortcutCode('q', KeyModifierSuper, current), kKeyNone);
  QCOMPARE(returnTypingShortcutCode('c', 0, current), kKeyNone);

  for (const auto &expected : kReturnTypingShortcutMap) {
    const auto code = returnTypingShortcutCode(expected.macKey, expected.macModifiers, current);
    QVERIFY(code != kKeyNone);
    QVERIFY(!isReturnTypingKey(code));
    QCOMPARE(returnTypingShortcut(code, current), &expected);
    QVERIFY(!returnTypingCodeOffered(code, 0));
  }
  const auto pasteCode = returnTypingShortcutCode('v', KeyModifierControl, current);
  QCOMPARE(returnTypingShortcut(pasteCode, current)->windowsModifiers, KeyModifierControl);
  QVERIFY(!returnTypingCodeOffered(kReturnTypingShortcutCodeBase + kReturnTypingShortcutMap.size(), current));
  QVERIFY(!returnTypingCodeOffered(kKeyDelete, current));
}

QTEST_MAIN(ServerTests)
