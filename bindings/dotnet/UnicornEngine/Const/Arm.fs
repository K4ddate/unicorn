// For Unicorn Engine. AUTO-GENERATED FILE, DO NOT EDIT

namespace UnicornEngine.Const

open System

[<AutoOpen>]
module Arm =

    // ARM CPU

    let UC_CPU_ARM_926 = 0
    let UC_CPU_ARM_946 = 1
    let UC_CPU_ARM_1026 = 2
    let UC_CPU_ARM_1136_R2 = 3
    let UC_CPU_ARM_1136 = 4
    let UC_CPU_ARM_1176 = 5
    let UC_CPU_ARM_11MPCORE = 6
    let UC_CPU_ARM_CORTEX_M0 = 7
    let UC_CPU_ARM_CORTEX_M3 = 8
    let UC_CPU_ARM_CORTEX_M4 = 9
    let UC_CPU_ARM_CORTEX_M7 = 10
    let UC_CPU_ARM_CORTEX_M33 = 11
    let UC_CPU_ARM_CORTEX_R5 = 12
    let UC_CPU_ARM_CORTEX_R5F = 13
    let UC_CPU_ARM_CORTEX_A7 = 14
    let UC_CPU_ARM_CORTEX_A8 = 15
    let UC_CPU_ARM_CORTEX_A9 = 16
    let UC_CPU_ARM_CORTEX_A15 = 17
    let UC_CPU_ARM_TI925T = 18
    let UC_CPU_ARM_SA1100 = 19
    let UC_CPU_ARM_SA1110 = 20
    let UC_CPU_ARM_PXA250 = 21
    let UC_CPU_ARM_PXA255 = 22
    let UC_CPU_ARM_PXA260 = 23
    let UC_CPU_ARM_PXA261 = 24
    let UC_CPU_ARM_PXA262 = 25
    let UC_CPU_ARM_PXA270 = 26
    let UC_CPU_ARM_PXA270A0 = 27
    let UC_CPU_ARM_PXA270A1 = 28
    let UC_CPU_ARM_PXA270B0 = 29
    let UC_CPU_ARM_PXA270B1 = 30
    let UC_CPU_ARM_PXA270C0 = 31
    let UC_CPU_ARM_PXA270C5 = 32
    let UC_CPU_ARM_MAX = 33
    let UC_CPU_ARM_ENDING = 34

    // ARM registers

    let UC_ARM_REG_INVALID = 0
    let UC_ARM_REG_APSR = 1
    let UC_ARM_REG_APSR_NZCV = 2
    let UC_ARM_REG_CPSR = 3
    let UC_ARM_REG_FPEXC = 4
    let UC_ARM_REG_FPINST = 5
    let UC_ARM_REG_FPSCR = 6
    let UC_ARM_REG_FPSCR_NZCV = 7
    let UC_ARM_REG_FPSID = 8
    let UC_ARM_REG_ITSTATE = 9
    let UC_ARM_REG_LR = 10
    let UC_ARM_REG_PC = 11
    let UC_ARM_REG_SP = 12
    let UC_ARM_REG_SPSR = 13
    let UC_ARM_REG_D0 = 14
    let UC_ARM_REG_D1 = 15
    let UC_ARM_REG_D2 = 16
    let UC_ARM_REG_D3 = 17
    let UC_ARM_REG_D4 = 18
    let UC_ARM_REG_D5 = 19
    let UC_ARM_REG_D6 = 20
    let UC_ARM_REG_D7 = 21
    let UC_ARM_REG_D8 = 22
    let UC_ARM_REG_D9 = 23
    let UC_ARM_REG_D10 = 24
    let UC_ARM_REG_D11 = 25
    let UC_ARM_REG_D12 = 26
    let UC_ARM_REG_D13 = 27
    let UC_ARM_REG_D14 = 28
    let UC_ARM_REG_D15 = 29
    let UC_ARM_REG_D16 = 30
    let UC_ARM_REG_D17 = 31
    let UC_ARM_REG_D18 = 32
    let UC_ARM_REG_D19 = 33
    let UC_ARM_REG_D20 = 34
    let UC_ARM_REG_D21 = 35
    let UC_ARM_REG_D22 = 36
    let UC_ARM_REG_D23 = 37
    let UC_ARM_REG_D24 = 38
    let UC_ARM_REG_D25 = 39
    let UC_ARM_REG_D26 = 40
    let UC_ARM_REG_D27 = 41
    let UC_ARM_REG_D28 = 42
    let UC_ARM_REG_D29 = 43
    let UC_ARM_REG_D30 = 44
    let UC_ARM_REG_D31 = 45
    let UC_ARM_REG_FPINST2 = 46
    let UC_ARM_REG_MVFR0 = 47
    let UC_ARM_REG_MVFR1 = 48
    let UC_ARM_REG_MVFR2 = 49
    let UC_ARM_REG_Q0 = 50
    let UC_ARM_REG_Q1 = 51
    let UC_ARM_REG_Q2 = 52
    let UC_ARM_REG_Q3 = 53
    let UC_ARM_REG_Q4 = 54
    let UC_ARM_REG_Q5 = 55
    let UC_ARM_REG_Q6 = 56
    let UC_ARM_REG_Q7 = 57
    let UC_ARM_REG_Q8 = 58
    let UC_ARM_REG_Q9 = 59
    let UC_ARM_REG_Q10 = 60
    let UC_ARM_REG_Q11 = 61
    let UC_ARM_REG_Q12 = 62
    let UC_ARM_REG_Q13 = 63
    let UC_ARM_REG_Q14 = 64
    let UC_ARM_REG_Q15 = 65
    let UC_ARM_REG_R0 = 66
    let UC_ARM_REG_R1 = 67
    let UC_ARM_REG_R2 = 68
    let UC_ARM_REG_R3 = 69
    let UC_ARM_REG_R4 = 70
    let UC_ARM_REG_R5 = 71
    let UC_ARM_REG_R6 = 72
    let UC_ARM_REG_R7 = 73
    let UC_ARM_REG_R8 = 74
    let UC_ARM_REG_R9 = 75
    let UC_ARM_REG_R10 = 76
    let UC_ARM_REG_R11 = 77
    let UC_ARM_REG_R12 = 78
    let UC_ARM_REG_S0 = 79
    let UC_ARM_REG_S1 = 80
    let UC_ARM_REG_S2 = 81
    let UC_ARM_REG_S3 = 82
    let UC_ARM_REG_S4 = 83
    let UC_ARM_REG_S5 = 84
    let UC_ARM_REG_S6 = 85
    let UC_ARM_REG_S7 = 86
    let UC_ARM_REG_S8 = 87
    let UC_ARM_REG_S9 = 88
    let UC_ARM_REG_S10 = 89
    let UC_ARM_REG_S11 = 90
    let UC_ARM_REG_S12 = 91
    let UC_ARM_REG_S13 = 92
    let UC_ARM_REG_S14 = 93
    let UC_ARM_REG_S15 = 94
    let UC_ARM_REG_S16 = 95
    let UC_ARM_REG_S17 = 96
    let UC_ARM_REG_S18 = 97
    let UC_ARM_REG_S19 = 98
    let UC_ARM_REG_S20 = 99
    let UC_ARM_REG_S21 = 100
    let UC_ARM_REG_S22 = 101
    let UC_ARM_REG_S23 = 102
    let UC_ARM_REG_S24 = 103
    let UC_ARM_REG_S25 = 104
    let UC_ARM_REG_S26 = 105
    let UC_ARM_REG_S27 = 106
    let UC_ARM_REG_S28 = 107
    let UC_ARM_REG_S29 = 108
    let UC_ARM_REG_S30 = 109
    let UC_ARM_REG_S31 = 110
    let UC_ARM_REG_C1_C0_2 = 111
    let UC_ARM_REG_C13_C0_2 = 112
    let UC_ARM_REG_C13_C0_3 = 113
    let UC_ARM_REG_IPSR = 114
    let UC_ARM_REG_MSP = 115
    let UC_ARM_REG_PSP = 116
    let UC_ARM_REG_CONTROL = 117
    let UC_ARM_REG_IAPSR = 118
    let UC_ARM_REG_EAPSR = 119
    let UC_ARM_REG_XPSR = 120
    let UC_ARM_REG_EPSR = 121
    let UC_ARM_REG_IEPSR = 122
    let UC_ARM_REG_PRIMASK = 123
    let UC_ARM_REG_BASEPRI = 124
    let UC_ARM_REG_BASEPRI_MAX = 125
    let UC_ARM_REG_FAULTMASK = 126
    let UC_ARM_REG_APSR_NZCVQ = 127
    let UC_ARM_REG_APSR_G = 128
    let UC_ARM_REG_APSR_NZCVQG = 129
    let UC_ARM_REG_IAPSR_NZCVQ = 130
    let UC_ARM_REG_IAPSR_G = 131
    let UC_ARM_REG_IAPSR_NZCVQG = 132
    let UC_ARM_REG_EAPSR_NZCVQ = 133
    let UC_ARM_REG_EAPSR_G = 134
    let UC_ARM_REG_EAPSR_NZCVQG = 135
    let UC_ARM_REG_XPSR_NZCVQ = 136
    let UC_ARM_REG_XPSR_G = 137
    let UC_ARM_REG_XPSR_NZCVQG = 138
    let UC_ARM_REG_CP_REG = 139
    let UC_ARM_REG_ESR = 140
    let UC_ARM_REG_ENDING = 141

    // alias registers
    let UC_ARM_REG_R13 = 12
    let UC_ARM_REG_R14 = 10
    let UC_ARM_REG_R15 = 11
    let UC_ARM_REG_SB = 75
    let UC_ARM_REG_SL = 76
    let UC_ARM_REG_FP = 77
    let UC_ARM_REG_IP = 78

