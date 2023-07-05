#include <gtest/gtest.h>

#include "../chip8.h"
#include "testing_roms.h"

#define __EMBED_ROM__ 1

namespace {
class Chip8Test : public ::testing::Test {
	protected:
		struct Chip8 chip8;
		void SetUp() override {
			resetChip8(&chip8);
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
		}
};
}

TEST_F(Chip8Test, TestSize) {
	loadROM("games/omnichip8", rom_omnichip8);
	ASSERT_EQ(chip8.romSize, rom_omnichip8_size);
}


GTEST_API_ int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}