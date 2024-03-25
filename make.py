#!/usr/bin/env python3

# This script replaces the Makefile to provide a convenient cross-platform build tool

import argparse
import glob
import os
from os import path
import shutil
import subprocess
import sys

inc_tmpl = """#ifndef ROM_EMBED_H
#define ROM_EMBED_H
#ifdef __EMBED_ROM__
unsigned char ROM[] = {{
	{}
}};
unsigned int ROM_len = {};
#endif
#endif
"""

def fatal_print(msg):
	print(msg)
	exit(1)

def create_embed(path: str):
	hex_str = ""
	with open(path, "rb") as f:
		bytes = f.read()
		a = 0
		for b in range(len(bytes)):
			hex_str += "0x%02x" % bytes[b]
			hex_str += ", " if b < len(bytes) - 1 else ""
			a += 1
			if a == 12:
				hex_str += "\n\t"
				a = 0

	with open("src/rom_embed.h", "wb") as r:
		r.write(bytearray(inc_tmpl.format(hex_str, len(bytes)), "utf-8"))

def fs_action(action, sourcefile, destfile = ""):
	isfile = path.isfile(sourcefile) or path.islink(sourcefile)
	isdir = path.isdir(sourcefile)
	if action == "copy":
		fs_action("delete", destfile)
		if isfile:
			shutil.copy(sourcefile, destfile)
		elif isdir:
			shutil.copytree(sourcefile, destfile)
	elif action == "move":
		fs_action("delete", destfile)
		shutil.move(sourcefile, destfile)
	elif action == "mkdir":
		if isfile:
			fs_action("delete", sourcefile)
		elif not isdir:
			os.makedirs(sourcefile)
	elif action == "delete":
		if isfile:
			os.remove(sourcefile)
		elif isdir:
			shutil.rmtree(sourcefile)
	else:
		raise Exception("Invalid action, must be 'copy', 'move', 'mkdir', or 'delete'")

def run_cmd(cmd, print_output = True, realtime = False, print_command = False):
	if print_command:
		print(cmd)

	proc = subprocess.Popen(cmd, stdout = subprocess.PIPE, stderr=subprocess.STDOUT, shell = True)
	output = ""
	status = 0
	if realtime: # print the command's output in real time, ignores print_output
		while True:
			realtime_output = proc.stdout.readline().decode("utf-8")
			if realtime_output == "" and status is not None:
				return ("", status)
			if realtime_output:
				print(realtime_output.strip())
				output += realtime_output
			status = proc.poll()
	else: # wait until the command is finished to print the output
		output = proc.communicate()[0]
		if output is not None:
			output = output.decode("utf-8").strip()
		else:
			output = ""
		status = proc.wait()
		if output != "" and print_output:
			print(output)
	if status is None:
		status = 0
	return (output, status)

def in_pathenv(file):
	paths = os.environ["PATH"].split(path.pathsep)
	for loc in paths:
		if path.isfile(path.join(loc, file)) or path.isfile(path.join(loc, file + ".exe")):
			return True
	return False

def out_file(platform, windows):
	oc8_out = "oc8"
	if windows and platform == "native":
		oc8_out += ".exe"
	elif platform in ("c64", "sim6502"):
		oc8_out += "-" + platform + ".prg"
	elif platform == "gb":
		oc8_out += ".gb"
	elif platform == "ti83":
		# TODO: make this actually useful
		oc8_out += ".83p"
		fatal_print("TI-8x support hasn't been implemented yet")
	elif platform == "emscripten":
		oc8_out += ".html"
	elif not "native":
		fatal_print("Unsupported platform " + platform)

	return oc8_out

def term_type():
	if "FrameworkVersion" in os.environ:
		return "msbuild"
	if "OS" in os.environ and "MINGW_PREFIX":
		return "mingw"
	if "TERM" in os.environ:
		return "unix"
	if "OS" in os.environ:
		return "cmd"
	return "other"

def build(platform = "native", library = "sdl", debugging = False, print_opcodes=False, embed = "", listing_file = ""):
	if embed != "":
		create_embed(embed)
	term = term_type()
	msbuild = term == "msbuild" and platform == "native"

	if platform == "native" and not msbuild and "TERM" not in os.environ:
		fatal_print(
			"You appear to be in Windows, but are not using the Visual Studio command prompt\n" +
			"or Cygwin. In order to build OmniChip-8 natively in Windows, you need to use\n" +
			"one of the two. Eventually, this build script may use something else as an\n" +
			"alternative"
		)

	oc8_out = out_file(platform, "OS" in os.environ and os.environ["OS"] == "Windows_NT")

	cmd = ""
	sources = "src/util.c src/opcode_funcs.c src/chip8.c src/main.c"

	if msbuild:
		# Visual Studio development console is being used
		if not in_pathenv("msbuild"):
			fatal_print("Unable to find msbuild.exe. Check your Visual Studio installation and try again")
		cmd = "msbuild.exe OmniChip-8.sln"
		if debugging:
			cmd += " -p:Configuration=Debug"
		else:
			cmd += " -p:Configuration=Release"
	elif platform == "native":
		if not in_pathenv("cc"):
			fatal_print("Unable to find the cc compiler")

		includes_path = "/usr/include"
		if term == "mingw":
			includes_path = "/mingw64/include"

		lib = "-lSDL2"
		if library == "curses":
			lib = "-lcurses"
		elif library == "sdl":
			includes_path += "/SDL2"

		sources += " src/io_{}.c".format(library)

		cflags = "-pedantic -Wall -std=c89 -D_POSIX_SOURCE -fdiagnostics-color=always "
		cmd = "cc -o {oc8_out} -D{io_const}_IO {includes_path} {debug_flag} {opcodes} {cflags} {sources} {lib}".format(
			oc8_out = oc8_out,
			io_const = library.upper(),
			includes_path = "-I" + includes_path,
			debug_flag = "-g" if debugging else "",
			opcodes = "-DPRINT_OPCODES" if print_opcodes else "",
			cflags = cflags,
			sources = sources,
			lib = lib
		)
	elif platform in ("c64", "sim6502"):
		if not in_pathenv("cl65"):
			fatal_print("Unable to find the cc65 development kit, required to build for 65xx targets")
		sources += " src/io_{}.c".format(platform)

		cmd = "cl65 -Oi -Or -Os {debug_flag} -o {oc8_out} -t {platform} {listing} -D{io_const}_IO -D__EMBED_ROM__ {sources}".format(
			debug_flag = "-g -Ln oc8.lbl" if debugging else "",
			oc8_out = oc8_out,
			platform = platform,
			listing = ("-l " + listing_file) if listing_file != "" else "",
			io_const = platform.upper(),
			sources = sources
		)
	elif platform in ("gb", "ti83"):
		if not in_pathenv("zcc"):
			fatal_print("Unable to find the z88dk development kit, required to build for GameBoy and TI-8x")
		io_const = platform.upper()

		sources += " src/io_{}.c".format(platform)
		cmd = "zcc +{platform} --opt-code-speed -create-app -o {oc8_out} -D{io_const}_IO -D__EMBED_ROM__ {sources}".format(
			platform = platform,
			oc8_out = oc8_out,
			io_const = io_const,
			sources = sources
		)
	elif platform == "emscripten":
		if not in_pathenv("emcc"):
			fatal_print("Unable to find the emscripten development kit, required to build browser-compatible JavaScript")
		sources += " src/io_{}.c".format("sdl")
		cmd = "emcc -o {oc8_out} -s --embed-file games USE_SDL=2 --shell-file shell.html -DSDL_IO -DEMSCRIPTEN_IO {sources}".format(
			oc8_out = oc8_out,
			sources = sources
		)
	else:
		fatal_print("Unsupported platform " + platform)

	status = run_cmd(cmd, print_output = True, realtime = True, print_command = True)
	if status[1] != 0:
		fatal_print("Failed building OmniChip-8, see command output for details")
	
	if msbuild:
		out_dir = path.join("./build/x64/", "Debug" if debugging else "Release")
		fs_action("copy", path.join(out_dir, "SDL2.dll"), "SDL2.dll")
		fs_action("copy", path.join(out_dir, "oc8.exe"), "oc8.exe")
		
	print("Built OmniChip-8 successfully")

def run_tests():
	test_commands = (
		"cmake -B build",
		"cmake --build build",
		"ctest --test-dir build",
		"cmake --build build -t lcov"
	)
	create_embed("games/omnichip8")
	for cmd in test_commands:
		status = run_cmd(cmd, realtime=True, print_command=True)
		if status[1] != 0:
			fatal_print("Failed running test command(s)")
	print("Tests completed without errors, coverage reports should be in ./build/lcov/html/")

def clean():
	print("Cleaning up")
	fs_action("delete", "build/")
	del_files = glob.glob("oc8*") + glob.glob("src/*.o") + ["zcc_opt.def", "SDL2.dll", "src/rom_embed.h", "x64", "packages", "build"]
	for del_file in del_files:
		fs_action("delete", del_file)

if __name__ == "__main__":
	actions = ("sdl", "curses", "gb", "c64", "sim6502", "test", "embed", "clean", "help")
	action = "sdl" if len(sys.argv) == 1 else sys.argv.pop(1)
	platform = "native"
	library = "sdl"
	parser = argparse.ArgumentParser(description = "OmniChip-8 build script")
	if action == "test":
		run_tests()
	elif action == "clean":
		clean()
	elif action == "help" or action == "--help" or action == "-h":
		joined = ", ".join(actions)
		print(f"usage: {sys.argv[0]} [action] [args]")
		print(f"    valid actions (default is sdl): {joined}")
	elif action in actions:
		if action == "embed":
			if len(sys.argv) != 2:
				fatal_print(f"usage: {sys.argv[0]} embed path/to/romfile")
			create_embed(sys.argv[1])
			exit()
		elif action == "sdl":
			library = action
			parser.add_argument( "-p", "--print-opcodes",
				help="Print opcodes on the command line as they are executed",
				default=False,
				action="store_true")
		elif action == "curses":
			library = action
		else:
			platform = action
			library = ""
		if action in ("c64", "gb"):
			parser.add_argument("--listing-file",
				help="Generate listing file",
				default="",
				metavar="PATH")
		parser.add_argument("--debug",
			help="Build OmniChip-8 with debugging symbols",
			default=False,
			action="store_true")
		parser.add_argument("--embed",
			help="embed a ROM file in OmniChip-8 for platforms that don't have file access (GameBoy, Commodore 64, etc)",
			default="games/omnichip8")
		args = parser.parse_args()

		create_embed(args.embed)
		build(platform, library,
			args.__dict__.get("debug", False),
			args.__dict__.get("print_opcodes", False),
			args.__dict__.get("embed", "games/omnichip8"),
			args.__dict__.get("listing_file", ""))
	else:
		fatal_print(f"Unrecognized action {action}, recognized actions: {actions}")
