//
// Translate 8-bit access from ARM to SRAM.
//
`timescale 1ns / 1ps

module sram_to_arm (
		inout [15:0] ARM_D,
		input [18:1] ARM_A,
		input ARM_NLB,
		input ARM_CS,
		input ARM_OE,
		input ARM_WE,

		inout [7:0] SRAM_D,
		output [18:0] SRAM_A,
		output SRAM_CS,
		output SRAM_OE,
		output SRAM_WE
	);

	assign SRAM_CS = ARM_CS;
	assign SRAM_OE = ARM_OE;
	assign SRAM_WE = ARM_WE;
	assign SRAM_A  = { ARM_A, ARM_NLB };

	assign ARM_D[15:0] = (!ARM_OE && !ARM_CS) ? { 8'h00, SRAM_D[7:0] } : 16'hzzzz;
	assign SRAM_D[7:0] = (!ARM_WE && !ARM_CS) ? ARM_D[7:0] : 8'hzz;

endmodule
