//
// Translate 8-bit access from ARM to SRAM.
//
`timescale 1ns / 1ps

module sram_to_arm (
		inout [15:0] ARM_D,
		input [18:0] ARM_A,
		input ARM_nLB,
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

	assign ARM_D[15:0] = (!ARM_nOE && !ARM_nCS) ? { 8'h00, SRAM_D[7:0] } : 16'hzzzz;
	assign SRAM_D[7:0] = (!ARM_nWE && !ARM_nCS) ? ARM_D[7:0] : 8'hzz;

endmodule
