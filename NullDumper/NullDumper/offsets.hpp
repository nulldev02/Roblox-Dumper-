#pragma once

#include <Windows.h>

// SkidClub Dumper V1.1

#define REBASE(x) (x + (uintptr_t)GetModuleHandle(nullptr))

namespace Offsets {

    const uintptr_t PushInstance2 = REBASE(0x4676640);
    const uintptr_t FakeDataModel = REBASE(0x6E03BC8);
    const uintptr_t Task_Spawn = REBASE(0x7271E0);
    const uintptr_t KTable = REBASE(0x7AE34F0);
    const uintptr_t ScriptContextResume = REBASE(0x1BC2460);
    const uintptr_t PushInstance = REBASE(0x1BCECA0);
    const uintptr_t Impersonator = REBASE(0x1BD3BA0);
    const uintptr_t Print = REBASE(0x1C8E7A0);
    const uintptr_t LuaO_nilobject = REBASE(0x5C6B9D8);
    const uintptr_t FireRightMouseClick = REBASE(0x23FC370);
    const uintptr_t luaD_throw = REBASE(0x3DC71E0);
    const uintptr_t Luau_Execute = REBASE(0x3DDF850);
    const uintptr_t LuaH_Dummynode = REBASE(0x5C6B0B8);
    const uintptr_t TaskScheduler = REBASE(0x827FF60);

}
