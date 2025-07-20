#include <gtest/gtest.h>
#ifdef CMAKE_TEST
extern "C" {
#endif
#include "../chip8.h"
#ifdef CMAKE_TEST
}
#endif
#include "testing_roms.h"

#define __EMBED_ROM__ 1

namespace {
class Chip8Test : public ::testing::Test {
	protected:
		uchar blankScreen[SCREEN_MEMORY];
		void SetUp() override {
			resetChip8();
			for(int i = 0; i < 80; i++) {
				chip8.memory[FONT_START_ADDR + i] = font[i];
			}
		}
		void TearDown() override {
			ASSERT_NE(chip8.status, STATUS_ERROR);
		}

		template <ushort S>
		void loadROM(const char* mockFilepath, uchar (&romBytes)[S]) {
			chip8.romPath = (char*)mockFilepath;
			chip8.romSize = sizeof(romBytes)/sizeof(romBytes[0]);
			printf("Loading %s (%d bytes)\n", mockFilepath, chip8.romSize);
			memcpy(chip8.memory + ROM_START_ADDR, romBytes, chip8.romSize);
		}

		bool isScreenBlank() {
			for(int y = 0; y < 32; y++) {
				for(int x = 0; x < 64; x++) {
					if(chip8.screen[x+y*64] != 0) {
						return false;
					}
				}
			}
			return true;
		}

		void printStack() {
			size_t stackSize = sizeof(chip8.stack) / sizeof(chip8.stack[0]);
			printf("CHIP-8 stack:\n");
			for(int i = 0; i < stackSize; i++) {
				printf("stack[%d]: %d\n", i, chip8.stack[i]);
			}
		}

		void step() {
			doCycle();
		}
};
}

TEST_F(Chip8Test, TestMacros) {
	loadROM("games/macros", rom_macros);
	ASSERT_EQ(chip8.romSize, rom_macros_size);
	ASSERT_EQ(chip8.PC, ROM_START_ADDR);
	ushort romWord = GET_WORD();;
	ASSERT_EQ(romWord, 0x0123);
	ASSERT_EQ(OPCODE_N(romWord), 3);
	ASSERT_EQ(OPCODE_NN(romWord), 0x23);
	ASSERT_EQ(OPCODE_NNN(romWord), 0x123);
	ASSERT_EQ(OPCODE_X(romWord), 1);
	ASSERT_EQ(OPCODE_Y(romWord), 2);
}

TEST_F(Chip8Test, TestSize) {
	loadROM("games/omnichip8", rom_omnichip8);
	ASSERT_EQ(chip8.romSize, rom_omnichip8_size);
}

TEST_F(Chip8Test, TestCLS) {
	// 00E0: clear screen
	loadROM("games/clearscreen", rom_blankScreen);
	ASSERT_EQ(chip8.romSize, rom_blankScreen_size);
	ASSERT_TRUE(isScreenBlank());
	chip8.screen[32] = 0x1; // put pixel in arbitrary location to be cleared
	ASSERT_FALSE(isScreenBlank());
	step();
	ASSERT_TRUE(isScreenBlank());
}

TEST_F(Chip8Test, TestRET) {
	// 00EE: return
	// 2nnn: call subroutine at nnn
	loadROM("games/callreturn", rom_call_return);
	ASSERT_EQ(chip8.romSize, rom_call_return_size);
	ASSERT_EQ(chip8.PC, 0x200);
	ASSERT_EQ(chip8.stackPointer, 0);
	step();
	ASSERT_EQ(chip8.PC, 0x204);
	ASSERT_EQ(chip8.stackPointer, 1);
	step();
	ASSERT_EQ(chip8.PC, 0x202);
	ASSERT_EQ(chip8.stackPointer, 0);
	step();
	ASSERT_EQ(chip8.PC, 0x200);
	ASSERT_EQ(chip8.stackPointer, 0);
	step();
	ASSERT_EQ(chip8.PC, 0x204);
	ASSERT_EQ(chip8.stackPointer, 1);
}

TEST_F(Chip8Test, TestJP) {
	// 1nnn: jump
	loadROM("games/jumpreturn", rom_jump_return);
	ASSERT_EQ(chip8.romSize, rom_jump_return_size);
	ASSERT_EQ(chip8.PC, 0x200);
	ASSERT_EQ(chip8.stackPointer, 0);
	step();
	ASSERT_EQ(chip8.stackPointer, 0);
	ASSERT_EQ(chip8.PC, 0x204);
}

TEST_F(Chip8Test, TestLD_EQ_NE) {
	// 3xkk: SE Vx, kk
	// 4xkk: SNE Vx kk
	// 5xy0: SE Vx Vy
	// 6xkk: LD Vx kk
	// 8xy0: LD Vx Vy
	// 9xy0: SNE Vx Vy
	loadROM("games/ldsesne", rom_ld_se_sne);
	ASSERT_EQ(chip8.romSize, rom_ld_se_sne_size);
	ASSERT_EQ(chip8.V[0x1], 0);
	step();
	ASSERT_EQ(chip8.V[0x1], 0xab);
	ASSERT_EQ(chip8.PC, 0x202);
	step();
	ASSERT_EQ(chip8.PC, 0x206);
	step();
	ASSERT_EQ(chip8.V[0x1], 0xaa);
	step();
	ASSERT_EQ(chip8.PC, 0x20a);
	step();
	ASSERT_EQ(chip8.V[0x1], 0xab);
	step();
	ASSERT_EQ(chip8.PC, 0x20e);
	step();
	ASSERT_EQ(chip8.V[0x1], 0xaa);
	step();
	ASSERT_EQ(chip8.PC, 0x212);
	step();
	step();
	ASSERT_EQ(chip8.PC, 0x218);
	step();
	step();
	ASSERT_EQ(chip8.V[0], chip8.V[8]);
}

TEST_F(Chip8Test, TestADD) {
	// 7xkk: ADD Vx kk
	// 8xy4: ADD Vx Vy
	loadROM("games/add", rom_add);
	ASSERT_EQ(chip8.romSize, rom_add_size);
	ASSERT_EQ(chip8.V[0], 0);
	step();
	step();
	ASSERT_EQ(chip8.V[0], 1);
	ASSERT_EQ(chip8.V[1], 1);
	step();
	ASSERT_EQ(chip8.V[0], 2);
	step();
	ASSERT_EQ(chip8.V[0], 3);
	step();
	step();
	ASSERT_EQ(chip8.V[1], 0xff);
	ASSERT_EQ(chip8.V[0xf], 1);
	ASSERT_EQ(chip8.V[0], 2);
}

TEST_F(Chip8Test, TestAND_OR_XOR) {
	// 8xy1: OR Vx Vy
	// 8xy2: AND Vx Vy
	// 8xy3: XOR Vx Vy
	loadROM("games/andorxor", rom_and_or_xor);
	ASSERT_EQ(chip8.romSize, rom_and_or_xor_size);
	step();
	ASSERT_EQ(chip8.V[0], 0x80);
	step();
	ASSERT_EQ(chip8.V[1], 0x1);
	step();
	ASSERT_EQ(chip8.V[0], 0x81);
	step();
	ASSERT_EQ(chip8.V[1], 0xf0);
	step();
	ASSERT_EQ(chip8.V[0], 0x80);
	step();
	ASSERT_EQ(chip8.V[1], 0x40);
	step();
	ASSERT_EQ(chip8.V[0], 0xc0);
	step();
	ASSERT_EQ(chip8.V[0], 0x80);
}

GTEST_API_ int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}