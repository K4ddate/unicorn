// For Unicorn Engine. AUTO-GENERATED FILE, DO NOT EDIT

package unicorn;

public interface S390xConst {

    // S390X CPU

    public static final int UC_CPU_S390X_Z900 = 0;
    public static final int UC_CPU_S390X_Z900_2 = 1;
    public static final int UC_CPU_S390X_Z900_3 = 2;
    public static final int UC_CPU_S390X_Z800 = 3;
    public static final int UC_CPU_S390X_Z990 = 4;
    public static final int UC_CPU_S390X_Z990_2 = 5;
    public static final int UC_CPU_S390X_Z990_3 = 6;
    public static final int UC_CPU_S390X_Z890 = 7;
    public static final int UC_CPU_S390X_Z990_4 = 8;
    public static final int UC_CPU_S390X_Z890_2 = 9;
    public static final int UC_CPU_S390X_Z990_5 = 10;
    public static final int UC_CPU_S390X_Z890_3 = 11;
    public static final int UC_CPU_S390X_Z9EC = 12;
    public static final int UC_CPU_S390X_Z9EC_2 = 13;
    public static final int UC_CPU_S390X_Z9BC = 14;
    public static final int UC_CPU_S390X_Z9EC_3 = 15;
    public static final int UC_CPU_S390X_Z9BC_2 = 16;
    public static final int UC_CPU_S390X_Z10EC = 17;
    public static final int UC_CPU_S390X_Z10EC_2 = 18;
    public static final int UC_CPU_S390X_Z10BC = 19;
    public static final int UC_CPU_S390X_Z10EC_3 = 20;
    public static final int UC_CPU_S390X_Z10BC_2 = 21;
    public static final int UC_CPU_S390X_Z196 = 22;
    public static final int UC_CPU_S390X_Z196_2 = 23;
    public static final int UC_CPU_S390X_Z114 = 24;
    public static final int UC_CPU_S390X_ZEC12 = 25;
    public static final int UC_CPU_S390X_ZEC12_2 = 26;
    public static final int UC_CPU_S390X_ZBC12 = 27;
    public static final int UC_CPU_S390X_Z13 = 28;
    public static final int UC_CPU_S390X_Z13_2 = 29;
    public static final int UC_CPU_S390X_Z13S = 30;
    public static final int UC_CPU_S390X_Z14 = 31;
    public static final int UC_CPU_S390X_Z14_2 = 32;
    public static final int UC_CPU_S390X_Z14ZR1 = 33;
    public static final int UC_CPU_S390X_GEN15A = 34;
    public static final int UC_CPU_S390X_GEN15B = 35;
    public static final int UC_CPU_S390X_QEMU = 36;
    public static final int UC_CPU_S390X_MAX = 37;
    public static final int UC_CPU_S390X_ENDING = 38;

    // S390X registers

    public static final int UC_S390X_REG_INVALID = 0;

    // General purpose registers
    public static final int UC_S390X_REG_R0 = 1;
    public static final int UC_S390X_REG_R1 = 2;
    public static final int UC_S390X_REG_R2 = 3;
    public static final int UC_S390X_REG_R3 = 4;
    public static final int UC_S390X_REG_R4 = 5;
    public static final int UC_S390X_REG_R5 = 6;
    public static final int UC_S390X_REG_R6 = 7;
    public static final int UC_S390X_REG_R7 = 8;
    public static final int UC_S390X_REG_R8 = 9;
    public static final int UC_S390X_REG_R9 = 10;
    public static final int UC_S390X_REG_R10 = 11;
    public static final int UC_S390X_REG_R11 = 12;
    public static final int UC_S390X_REG_R12 = 13;
    public static final int UC_S390X_REG_R13 = 14;
    public static final int UC_S390X_REG_R14 = 15;
    public static final int UC_S390X_REG_R15 = 16;

    // Floating point registers
    public static final int UC_S390X_REG_F0 = 17;
    public static final int UC_S390X_REG_F1 = 18;
    public static final int UC_S390X_REG_F2 = 19;
    public static final int UC_S390X_REG_F3 = 20;
    public static final int UC_S390X_REG_F4 = 21;
    public static final int UC_S390X_REG_F5 = 22;
    public static final int UC_S390X_REG_F6 = 23;
    public static final int UC_S390X_REG_F7 = 24;
    public static final int UC_S390X_REG_F8 = 25;
    public static final int UC_S390X_REG_F9 = 26;
    public static final int UC_S390X_REG_F10 = 27;
    public static final int UC_S390X_REG_F11 = 28;
    public static final int UC_S390X_REG_F12 = 29;
    public static final int UC_S390X_REG_F13 = 30;
    public static final int UC_S390X_REG_F14 = 31;
    public static final int UC_S390X_REG_F15 = 32;

    // Not real registers, low half of vr16-vr31
    public static final int UC_S390X_REG_F16 = 33;
    public static final int UC_S390X_REG_F17 = 34;
    public static final int UC_S390X_REG_F18 = 35;
    public static final int UC_S390X_REG_F19 = 36;
    public static final int UC_S390X_REG_F20 = 37;
    public static final int UC_S390X_REG_F21 = 38;
    public static final int UC_S390X_REG_F22 = 39;
    public static final int UC_S390X_REG_F23 = 40;
    public static final int UC_S390X_REG_F24 = 41;
    public static final int UC_S390X_REG_F25 = 42;
    public static final int UC_S390X_REG_F26 = 43;
    public static final int UC_S390X_REG_F27 = 44;
    public static final int UC_S390X_REG_F28 = 45;
    public static final int UC_S390X_REG_F29 = 46;
    public static final int UC_S390X_REG_F30 = 47;
    public static final int UC_S390X_REG_F31 = 48;

    // Access registers
    public static final int UC_S390X_REG_A0 = 49;
    public static final int UC_S390X_REG_A1 = 50;
    public static final int UC_S390X_REG_A2 = 51;
    public static final int UC_S390X_REG_A3 = 52;
    public static final int UC_S390X_REG_A4 = 53;
    public static final int UC_S390X_REG_A5 = 54;
    public static final int UC_S390X_REG_A6 = 55;
    public static final int UC_S390X_REG_A7 = 56;
    public static final int UC_S390X_REG_A8 = 57;
    public static final int UC_S390X_REG_A9 = 58;
    public static final int UC_S390X_REG_A10 = 59;
    public static final int UC_S390X_REG_A11 = 60;
    public static final int UC_S390X_REG_A12 = 61;
    public static final int UC_S390X_REG_A13 = 62;
    public static final int UC_S390X_REG_A14 = 63;
    public static final int UC_S390X_REG_A15 = 64;
    public static final int UC_S390X_REG_PC = 65;
    public static final int UC_S390X_REG_PSWM = 66;

    // pseudo registers, high half of vr16-vr31
    public static final int UC_S390X_REG_F0_HI = 67;
    public static final int UC_S390X_REG_F1_HI = 68;
    public static final int UC_S390X_REG_F2_HI = 69;
    public static final int UC_S390X_REG_F3_HI = 70;
    public static final int UC_S390X_REG_F4_HI = 71;
    public static final int UC_S390X_REG_F5_HI = 72;
    public static final int UC_S390X_REG_F6_HI = 73;
    public static final int UC_S390X_REG_F7_HI = 74;
    public static final int UC_S390X_REG_F8_HI = 75;
    public static final int UC_S390X_REG_F9_HI = 76;
    public static final int UC_S390X_REG_F10_HI = 77;
    public static final int UC_S390X_REG_F11_HI = 78;
    public static final int UC_S390X_REG_F12_HI = 79;
    public static final int UC_S390X_REG_F13_HI = 80;
    public static final int UC_S390X_REG_F14_HI = 81;
    public static final int UC_S390X_REG_F15_HI = 82;
    public static final int UC_S390X_REG_F16_HI = 83;
    public static final int UC_S390X_REG_F17_HI = 84;
    public static final int UC_S390X_REG_F18_HI = 85;
    public static final int UC_S390X_REG_F19_HI = 86;
    public static final int UC_S390X_REG_F20_HI = 87;
    public static final int UC_S390X_REG_F21_HI = 88;
    public static final int UC_S390X_REG_F22_HI = 89;
    public static final int UC_S390X_REG_F23_HI = 90;
    public static final int UC_S390X_REG_F24_HI = 91;
    public static final int UC_S390X_REG_F25_HI = 92;
    public static final int UC_S390X_REG_F26_HI = 93;
    public static final int UC_S390X_REG_F27_HI = 94;
    public static final int UC_S390X_REG_F28_HI = 95;
    public static final int UC_S390X_REG_F29_HI = 96;
    public static final int UC_S390X_REG_F30_HI = 97;
    public static final int UC_S390X_REG_F31_HI = 98;

    // float control register
    public static final int UC_S390X_REG_FPC = 99;

    // control registers
    public static final int UC_S390X_REG_CR0 = 100;
    public static final int UC_S390X_REG_CR1 = 101;
    public static final int UC_S390X_REG_CR2 = 102;
    public static final int UC_S390X_REG_CR3 = 103;
    public static final int UC_S390X_REG_CR4 = 104;
    public static final int UC_S390X_REG_CR5 = 105;
    public static final int UC_S390X_REG_CR6 = 106;
    public static final int UC_S390X_REG_CR7 = 107;
    public static final int UC_S390X_REG_CR8 = 108;
    public static final int UC_S390X_REG_CR9 = 109;
    public static final int UC_S390X_REG_CR10 = 110;
    public static final int UC_S390X_REG_CR11 = 111;
    public static final int UC_S390X_REG_CR12 = 112;
    public static final int UC_S390X_REG_CR13 = 113;
    public static final int UC_S390X_REG_CR14 = 114;
    public static final int UC_S390X_REG_CR15 = 115;
    public static final int UC_S390X_REG_ENDING = 116;

    // Alias registers

}
