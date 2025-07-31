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
#ifdef EMBED_ROM
unsigned char ROM[] = {{
	{}
}};
unsigned int ROM_len = {};
#endif
#endif
"""

def fatal_print(msg, stderr = True, exit_code = 1):
	if stderr:
		print(msg, file=sys.stderr)
	else:
		print(msg)
	exit(exit_code)

def create_embed(path: str):
	hex_str = ""
	with open(path, "rb") as f:
		bytes = f.read()
		for b in range(len(bytes)):
			hex_str += "0x%02x" % bytes[b]
			hex_str += ", " if b < len(bytes) - 1 else ""
			if b % 12 == 11:
				hex_str += "\n\t"

	with open("src/rom_embed.h", "wb") as r:
		r.write(bytearray(inc_tmpl.format(hex_str, len(bytes)), "utf-8"))

def fs_action(action, sourcefile, destfile = "", verbose = False):
	isfile = path.isfile(sourcefile) or path.islink(sourcefile)
	isdir = path.isdir(sourcefile)
	if action == "copy":
		fs_action("delete", destfile)
		if verbose:
			print(f"Copying file {sourcefile}{"/" if isdir and not destfile.endswith("/") else ""} to {destfile}")
		if isfile:
			shutil.copy(sourcefile, destfile)
		elif isdir:
			shutil.copytree(sourcefile, destfile)
	elif action == "move":
		if verbose:
			print(f"Moving file {sourcefile}{"/" if isdir and not destfile.endswith("/") else ""} to {destfile}")
		fs_action("delete", destfile)
		shutil.move(sourcefile, destfile)
	elif action == "mkdir":
		if verbose:
			print(f"Creating directory {sourcefile}")
		if isfile:
			fs_action("delete", sourcefile)
		elif not isdir:
			os.makedirs(sourcefile)
	elif action == "delete":
		if verbose:
			print(f"Deleting {sourcefile}")
		if isfile:
			os.remove(sourcefile)
		elif isdir:
			shutil.rmtree(sourcefile)
	else:
		raise Exception("Invalid action, must be 'copy', 'move', 'mkdir', or 'delete'")

def run_cmd(cmd, print_output = True, realtime = False, print_command = False, encoding = "utf-8", encoding_errors = "replace"):
	if print_command:
		print(cmd)

	proc = subprocess.Popen(cmd, stdout = subprocess.PIPE, stderr=subprocess.STDOUT, shell = True)
	output = ""
	status = 0
	if realtime and proc.stdout is not None: # print the command's output in real time, ignores print_output
		while True:
			realtime_output = proc.stdout.readline().decode(encoding, errors=encoding_errors)
			if realtime_output == "" and status is not None:
				return ("", status)
			if realtime_output:
				print(realtime_output.strip())
				output += realtime_output
			status = proc.poll()
	else: # wait until the command is finished to print the output
		output = proc.communicate()[0]
		if output is not None:
			output = output.decode(encoding, errors=encoding_errors).strip()
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
		print("WARNING: TI-8x support is still experimental")
		oc8_out += ".83p"
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

def make_disk(target:str, prg_file:str, rom_file:str):
	match target:
		case "c64":
			disk_img = f"oc8-{target}.d64"
			print(f"Creating disk image {disk_img} with the compiled program ROM file {rom_file}")

			status = run_cmd(f"c1541 -format oc8-{target},01 d64 {disk_img}")
			if status[1] != 0:
				fatal_print("Failed creating disk image, see command output for details")
			
			prg_noext = path.splitext(path.basename(prg_file))[0].lower()
			status = run_cmd(f"c1541 {disk_img} -write {prg_file} {prg_noext}", print_output=True, realtime=True, print_command=True)
			if status[1] != 0:
				fatal_print(f"Failed writing {prg_file} to disk image, see command output for details")

			status = run_cmd(f"c1541 {disk_img} -write {rom_file} {path.basename(rom_file).lower()}", print_output=True, realtime=True, print_command=True)
			if status[1] != 0:
				fatal_print(f"Failed writing {rom_file} to disk image, see command output for details")
			print(f"Disk image {disk_img} created successfully")
		case _:
			fatal_print(f"Disk creation is only supported for the c64 target, got {target}")

def build(platform = "native", library = "sdl", debugging = False, debug_keys=False, embed = "", listing_file = "", rom_on_disk = False):
	embed_disk = embed != "" and rom_on_disk
	if embed != "" and not rom_on_disk:
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

	if rom_on_disk:
		if platform != "c64":
			fatal_print("disk writing is only supported for the Commodore 64 platform")
		elif embed == "":
			fatal_print("You must specify a ROM file to embed in the disk image with --embed")

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

		sources += f" src/io_{library}.c"

		cflags = "-pedantic -Wall -std=c89 -D_POSIX_SOURCE -fdiagnostics-color=always "
		debug_flag = "-g" if debugging else ""
		debug_keys = "-DDEBUG_KEYS" if debug_keys else ""
		cmd = f"cc -o {oc8_out} -D{library.upper()}_IO -I{includes_path} {debug_flag} {debug_keys} {cflags} {sources} {lib}"
	elif platform in ("c64", "sim6502"):
		if not in_pathenv("cl65"):
			fatal_print("Unable to find the cc65 development kit, required to build for 65xx targets")
		sources += f" src/io_{platform}.c"
		if platform == "sim6502":
			sources += " asm/sim6502_time.s"

		debug_flag = "-g" if debugging else ""
		listing = f"-l {listing_file}" if listing_file != "" else ""
		cmd = f"cl65 -Osir {debug_flag} -o {oc8_out} -t {platform} -Cl {listing} -D{platform.upper()}_IO"
		cmd += f" -DEMBED_ROM=\"{embed}\" {sources}" if not embed_disk else f" {sources}"
	elif platform in ("gb", "ti83", "ti83"):
		if not in_pathenv("zcc"):
			fatal_print("Unable to find the z88dk development kit, required to build for GameBoy and TI-8x")
		io_const = platform.upper()

		sources += f" src/io_{platform}.c"
		cmd = f"zcc +{platform} --opt-code-speed -create-app -o {oc8_out} -D{io_const}_IO -DEMBED_ROM {sources}"
	elif platform == "emscripten":
		if not in_pathenv("emcc"):
			fatal_print("Unable to find the emscripten development kit, required to build browser-compatible JavaScript")
		sources += " src/io_sdl.c"
		cmd = f"emcc -o {oc8_out} -s USE_SDL=2 --shell-file shell.html -DSDL_IO -DEMBED_ROM={embed} -DEMSCRIPTEN_IO {sources}"
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

	if rom_on_disk:
		make_disk(platform, oc8_out, embed)

def run_tests(coverage = True):
	test_commands = [
		"cmake -B build",
		"cmake --build build",
		"ctest --test-dir build"
	]
	if coverage:
		test_commands.append("cmake --build build -t lcov")
	create_embed("games/omnichip8")
	for cmd in test_commands:
		status = run_cmd(cmd, realtime=True, print_command=True)
		if status[1] != 0:
			fatal_print("Failed running test command(s)")
	print("Tests completed without errors" + (", coverage reports should be in ./build/lcov/html/" if coverage else ""))

def clean():
	print("Cleaning up")
	fs_action("delete", "build/", verbose=True)
	del_files = glob.glob("oc8*") + glob.glob("src/*.o") + ["zcc_opt.def", "SDL2.dll", "src/rom_embed.h", "x64", "packages", "build", "Testing"]
	for del_file in del_files:
		fs_action("delete", del_file)

if __name__ == "__main__":
	actions = ("sdl", "curses", "emscripten", "gb", "c64", "sim6502", "ti83", "test", "embed", "clean", "help")
	action = "sdl" if len(sys.argv) == 1 else sys.argv.pop(1)
	platform = "native"
	library = "sdl"
	parser = argparse.ArgumentParser(description = "OmniChip-8 build script")
	if action == "test":
		parser.add_argument("--no-coverage",
			action="store_true",
			help="Run tests without generating a coverage report")
		args = parser.parse_args()
		run_tests(not args.no_coverage)
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
		elif action in ("sdl", "curses"):
			library = action
			parser.add_argument("-d", "--debug-keys",
				help="Enable play/pause (F1), reset (F2), step in (F3), step out (F4), print current address, opcode, and instruction info (F5), and print current interpreter status (F6)",
				action="store_true")
		elif action == "emscripten":
			platform = "emscripten"
			library = "sdl"
		elif action == "c64":
			parser.add_argument("-d", "--disk",
				help="If set, create a disk image with the compiled program ROM file specified by --embed instead of storing it in the PRG file",
				default=False,
				action="store_true")
			platform = "c64"
			library = ""
		else:
			platform = action
			library = ""
		if action in ("c64", "gb"):
			parser.add_argument("--listing-file",
				help="Generate listing file",
				default="",
				metavar="PATH")
		parser.add_argument("-g", "--debug-symbols",
			help="Build OmniChip-8 with debugging symbols",
			default=False,
			action="store_true")
		parser.add_argument("--embed",
			help="embed a ROM file in OmniChip-8 for platforms that don't have file access (GameBoy, Commodore 64, etc)",
			default="games/omnichip8")
		args = parser.parse_args()

		build(platform, library,
			args.__dict__.get("debug_symbols", False),
			args.__dict__.get("debug_keys", False),
			args.__dict__.get("embed", "games/omnichip8"),
			args.__dict__.get("listing_file", ""),
			args.__dict__.get("disk", False))
	else:
		fatal_print(f"Unrecognized action {action}, recognized actions: {', '.join(actions)}")
