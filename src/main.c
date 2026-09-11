#include "modding.h"
#include "ultra64.h"
#include "enums.h"
#include "common_structs.h"


#define LANKY_MAX_TIER 3  // Max movement animation type
#define LANKY_MAX_SWAY_RATE 1.5f // Cap at arm sway speed

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif


extern PlayerAdditionalActorData *extra_player_info_pointer;
extern Actor *gCurrentActorPointer;
extern Maps current_map;
extern u8 cc_number_of_players;
extern u8 is_cutscene_active;
extern u8 current_character_index[];
extern u8 cc_player_index;
extern s16 D_global_asm_807FD584;   // kong index
extern f32 D_global_asm_80753170[]; // idle threshold
extern f32 D_global_asm_8075318C[]; // creep -> walk
extern f32 D_global_asm_807531A8[]; // walk -> creep
extern f32 D_global_asm_807531C4[]; // walk -> run
extern f32 D_global_asm_807531E0[]; // run -> walk


extern void func_global_asm_806D03BC(void);
extern void playAnimation(Actor *arg0, s32 arg1);
extern void playActorAnimation(Actor *arg0, s32 arg1);
extern void func_global_asm_80614D00(Actor *arg0, f32 arg1, f32 arg2);
extern void func_global_asm_80613AF8(Actor *arg0, s32 arg1, f32 arg2, f32 arg3);
extern void func_global_asm_80613C48(Actor *arg0, s16 arg1, f32 arg2, f32 arg3);
extern void func_global_asm_80613A50(Actor *arg0, s8 arg1);
extern void func_global_asm_80614D90(Actor *arg0);

// Movement type patch
RECOMP_PATCH s16 func_global_asm_806CD9A0(Actor *arg0, f32 arg1, f32 arg2, s16 arg3, s16 arg4) {
    f32 var_f0 = MAX(arg1, arg2);

    if ((arg0->unk6A | arg0->unk6C) & 1) {
        switch (arg3) {
            case 0:
            case 1:
            case 2:
                if (arg2 != 0.0f) {
                    if (arg2 < D_global_asm_8075318C[arg4]) {
                        arg3 = 3;
                    } else if (arg2 < D_global_asm_807531C4[arg4]) {
                        arg3 = 4;
                    } else {
                        arg3 = 5;
                    }
                }
                break;
            case 3:
                if ((arg2 == 0.0f) && (arg1 < D_global_asm_80753170[arg4])) {
                    arg3 = 0;
                } else if (D_global_asm_807531C4[arg4] < arg2) {
                    arg3 = 5;
                } else if (D_global_asm_8075318C[arg4] < arg2) {
                    arg3 = 4;
                }
                break;
            case 4:
                if (var_f0 < D_global_asm_807531A8[arg4]) {
                    if (arg2 < D_global_asm_80753170[arg4]) {
                        arg3 = 0;
                    } else {
                        arg3 = 3;
                    }
                } else if (D_global_asm_807531C4[arg4] < arg2) {
                    arg3 = 5;
                }
                break;
            case 5:
                if (var_f0 < D_global_asm_807531E0[arg4]) {
                    arg3 = 4;
                }
                break;
            default:
                if (arg1 < D_global_asm_80753170[arg4]) {
                    arg3 = 0;
                } else if (arg1 < D_global_asm_807531A8[arg4]) {
                    arg3 = 3;
                } else {
                    if (arg1 < D_global_asm_807531E0[arg4]) {
                        arg3 = 4;
                    } else {
                        arg3 = 5;
                    }
                }
                break;
        }
        // mod
        if ((arg0->unk58 == ACTOR_LANKY) && (arg3 > LANKY_MAX_TIER)) {
            arg3 = LANKY_MAX_TIER;
        }
        // end mod
        if ((extra_player_info_pointer->unk1F0 & 0x100) && ((arg3 == 4) || (arg3 == 5))) {
            arg3 = 3;
        }
        if ((arg3 == 0) && ((current_map == MAP_FUNKYS_STORE) || (current_map == MAP_CRANKYS_LAB) || (current_map == MAP_CANDYS_MUSIC_SHOP) || (current_map == MAP_SNIDES_HQ))) {
            arg3 = 2;
        } else if ((arg3 == 0) && ((cc_number_of_players >= 2) || (extra_player_info_pointer->unk1F0 & 0x40) || !(arg0->object_properties_bitfield & 4) || (is_cutscene_active == 1))) {
            arg3 = 1;
        } else if ((arg3 == 1) && (cc_number_of_players < 2) && !(extra_player_info_pointer->unk1F0 & 0x40) && (arg0->object_properties_bitfield & 4) && (is_cutscene_active != 1)) {
            arg3 = 0;
        }
    } else {
        arg3 = 6;
    }
    return arg3;
}

// Arm sway speed patch
RECOMP_PATCH s32 func_global_asm_806CDD24(Actor *arg0, f32 arg1, f32 arg2, s32 arg3) {
    s16 var_v1;
    u8 pad[4];
    s16 sp2E;
    s16 sp2C;
    f32 var_f0;

    var_v1 = arg0->control_state;
    sp2E = extra_player_info_pointer->unk4C;
    sp2C = func_global_asm_806CD9A0(arg0, arg1, arg2, extra_player_info_pointer->unk4C, D_global_asm_807FD584);
    if (sp2C != 6) {
        arg0->y_velocity = 0.0f;
    }
    if (sp2E < 0) {
        func_global_asm_806D03BC();
    }
    if (sp2E != sp2C) {
        switch (sp2C) {
            case 0:
                if (extra_player_info_pointer->unk1F0 & 0x100) {
                    playActorAnimation(arg0, 0x1AD);
                } else {
                    playAnimation(arg0, 9);
                }
                func_global_asm_80614D00(arg0, 1.0f, 0.0f);
                var_v1 = 0xC;
                break;
            case 1:
                if (extra_player_info_pointer->unk1F0 & 0x100) {
                    playActorAnimation(arg0, 0x1AD);
                } else {
                    playActorAnimation(arg0, 0x13);
                }
                func_global_asm_80614D00(arg0, 1.0f, 0.0f);
                var_v1 = 0xC;
                break;
            case 2:
                playAnimation(arg0, 0x87);
                func_global_asm_80614D00(arg0, 1.0f, 0.0f);
                var_v1 = 0xC;
                break;
            case 3:
                if (extra_player_info_pointer->unk1F0 & 0x100) {
                    playActorAnimation(arg0, 0x76);
                    func_global_asm_80613C48(arg0, 0x2AC, 0.0f, 6.0f);
                } else {
                    func_global_asm_80613AF8(arg0, 2, 0.0f, 6.0f);
                    playAnimation(arg0, 1);
                }
                switch (sp2E) {
                    case 4:
                    case 5:
                        func_global_asm_80613A50(arg0, 2);
                        break;
                }
                var_v1 = 0xD;
                break;
            case 4:
                playAnimation(arg0, 2);
                func_global_asm_80613AF8(arg0, 3, 0.0f, 6.0f);
                if (sp2E == 3) {
                    switch (current_character_index[cc_player_index]) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            func_global_asm_80613A50(arg0, 4);
                            break;
                    }
                }
                if (sp2E == 5) {
                    func_global_asm_80613A50(arg0, 2);
                }
                var_v1 = 0xD;
                break;
            case 5:
                playAnimation(arg0, 3);
                func_global_asm_80613AF8(arg0, 4, 0.0f, 6.0f);
                if ((sp2E == 3) || (sp2E == 4)) {
                    func_global_asm_80613A50(arg0, 4);
                }
                var_v1 = 0xD;
                break;
            case 6:
                playAnimation(arg0, 0x1B);
                func_global_asm_80614D90(arg0);
                var_v1 = 0x1E;
                gCurrentActorPointer->unk9C = gCurrentActorPointer->y_position;
                break;
        }
        extra_player_info_pointer->unk4C = sp2C;
    }
    if (extra_player_info_pointer->unk4C >= 3) {
        if (extra_player_info_pointer->unk26 == 0x64) {
            var_f0 = arg0->unkB8;
        } else {
            var_f0 = extra_player_info_pointer->unk4;
        }
        var_f0 *= 0.025;
        if (extra_player_info_pointer->unk4C == 5) {
            var_f0 *= 0.6;
        }
        if (var_f0 > 1.25f) {
            var_f0 = var_f0 - ((var_f0 - 1.25f) * 0.4);
        }
        // mod
        if ((arg0->unk58 == ACTOR_LANKY) && (var_f0 > LANKY_MAX_SWAY_RATE)) {
            var_f0 = LANKY_MAX_SWAY_RATE;
        }
        // end mod
        if (arg0->animation_state->unk0->unk24 != 0.0f) {
            if (extra_player_info_pointer->unk26 == 0x64) {
                func_global_asm_80614D00(arg0, var_f0, 2.0f);
            } else {
                if (var_f0 < 0.5) {
                    var_f0 = 0.5f;
                }
                func_global_asm_80614D00(arg0, var_f0, 10.0f);
            }
        }
    }
    return var_v1;
}
