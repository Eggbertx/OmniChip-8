#include <gtest/gtest.h>

#include "../chip8.h"
#include "testing_roms.h"

#define __EMBED_ROM__ 1

namespace {
class Chip8Test : public ::testing::Test {
	protected:
		struct Chip8 chip8;
		uchar blankScreen[64 * 32];
		void SetUp() override {
			resetChip8(&chip8);
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
			chip8.romBytes = romBytes;
			chip8.romSize = sizeof(romBytes)/sizeof(romBytes[0]);
			printf("Loading %s (%d bytes)\n", mockFilepath, chip8.romSize);
			for(int i = 0; i < chip8.romSize; i++) {
				chip8.memory[ROM_START_ADDR + i] = chip8.romBytes[i];
			}
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
			doCycle(&chip8, 1);
		}
};
}

TEST_F(Chip8Test, TestSize) {
	loadROM("games/omnichip8", rom_omnichip8);
	ASSERT_EQ(chip8.romSize, rom_omnichip8_size);
}



TEST_F(Chip8Test, TestCLS) {
	loadROM("games/clearscreen", rom_blankScreen);
	ASSERT_EQ(chip8.romSize, rom_blankScreen_size);
	ASSERT_TRUE(isScreenBlank());
	chip8.screen[32] = 0x1; // put pixel in arbitrary location to be cleared
	ASSERT_FALSE(isScreenBlank());
	step();
	ASSERT_TRUE(isScreenBlank());
}

TEST_F(Chip8Test, TestRET) {
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

GTEST_API_ int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}