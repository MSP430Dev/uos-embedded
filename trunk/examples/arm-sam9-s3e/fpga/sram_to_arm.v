//
// Translate 8-bit access from ARM to SRAM.
//
`timescale 1ns / 1ps

module sram_to_arm (ARM_D, ARM_A, ARM_NLB, ARM_CS, ARM_OE, ARM_WE,
	SRAM_D, SRAM_A, SRAM_CS, SRAM_OE, SRAM_WE);

	inout [7:0] ARM_D;
	input [18:1] ARM_A;
	input ARM_NLB;
	input ARM_CS;
	input ARM_OE;
	input ARM_WE;
	inout [7:0] SRAM_D;
	output [18:0] SRAM_A;
	output SRAM_CS;
	output SRAM_OE;
	output SRAM_WE;

//	wire SRAM_CS;
//	wire SRAM_OE;
//	wire SRAM_WE;
//	wire [7:0] SRAM_D;
//	wire [18:0] SRAM_A;
//	wire [7:0] ARM_D;

	assign SRAM_CS = ARM_CS;
	assign SRAM_OE = ARM_OE;
	assign SRAM_WE = ARM_WE;
	assign SRAM_A  = { ARM_A, ARM_NLB };

	assign ARM_D[7:0] = (!ARM_OE && !ARM_CS) ? SRAM_D[7:0] : 8'hzz;
	assign SRAM_D[7:0] = (!ARM_WE && !ARM_CS) ? ARM_D[7:0] : 8'hzz;

endmodule
