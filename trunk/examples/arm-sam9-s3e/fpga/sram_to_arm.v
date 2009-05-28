//
// Translate 8-bit access from ARM to SRAM.
//
`timescale 1ns / 1ps

module sram_to_arm (
		inout [7:0] ARM_D,
		input [18:0] ARM_A,
		input ARM_nCS,
		input ARM_nOE,
		input ARM_nWE,

		inout [7:0] SRAM_D,
		output [18:0] SRAM_A,
		output SRAM_nCS,
		output SRAM_nOE,
		output SRAM_nWE
	);

	assign SRAM_nCS = ARM_nCS;
	assign SRAM_nOE = ARM_nOE;
	assign SRAM_nWE = ARM_nWE;
	assign SRAM_A   = ARM_A;

	assign ARM_D  = (!ARM_nOE && !ARM_nCS) ? SRAM_D : 8'hzz;
	assign SRAM_D = (!ARM_nWE && !ARM_nCS) ? ARM_D  : 8'hzz;

endmodule
