; _clock_gettime stub for sim6502

	.include "time.inc"
	.import return0
	.export _clock_gettime


.code

.proc _clock_gettime
	jmp return0
.endproc