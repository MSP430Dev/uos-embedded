;
; AVR microcontroller can have up to 56 interrupt vectors.
; Define all of them here.
; Macro __NVECTORS contains a number of vectors.
;

#if __NVECTORS > 0
		JMP(_intr0)
#endif
#if __NVECTORS > 1
		JMP(_intr1)
#endif
#if __NVECTORS > 2
		JMP(_intr2)
#endif
#if __NVECTORS > 3
		JMP(_intr3)
#endif
#if __NVECTORS > 4
		JMP(_intr4)
#endif
#if __NVECTORS > 5
		JMP(_intr5)
#endif
#if __NVECTORS > 6
		JMP(_intr6)
#endif
#if __NVECTORS > 7
		JMP(_intr7)
#endif
#if __NVECTORS > 8
		JMP(_intr8)
#endif
#if __NVECTORS > 9
		JMP(_intr9)
#endif
#if __NVECTORS > 10
		JMP(_intr10)
#endif
#if __NVECTORS > 11
		JMP(_intr11)
#endif
#if __NVECTORS > 12
		JMP(_intr12)
#endif
#if __NVECTORS > 13
		JMP(_intr13)
#endif
#if __NVECTORS > 14
		JMP(_intr14)
#endif
#if __NVECTORS > 15
		JMP(_intr15)
#endif
#if __NVECTORS > 16
		JMP(_intr16)
#endif
#if __NVECTORS > 17
		JMP(_intr17)
#endif
#if __NVECTORS > 18
		JMP(_intr18)
#endif
#if __NVECTORS > 19
		JMP(_intr19)
#endif
#if __NVECTORS > 20
		JMP(_intr20)
#endif
#if __NVECTORS > 21
		JMP(_intr21)
#endif
#if __NVECTORS > 22
		JMP(_intr22)
#endif
#if __NVECTORS > 23
		JMP(_intr23)
#endif
#if __NVECTORS > 24
		JMP(_intr24)
#endif
#if __NVECTORS > 25
		JMP(_intr25)
#endif
#if __NVECTORS > 26
		JMP(_intr26)
#endif
#if __NVECTORS > 27
		JMP(_intr27)
#endif
#if __NVECTORS > 28
		JMP(_intr28)
#endif
#if __NVECTORS > 29
		JMP(_intr29)
#endif
#if __NVECTORS > 30
		JMP(_intr30)
#endif
#if __NVECTORS > 31
		JMP(_intr31)
#endif
#if __NVECTORS > 32
		JMP(_intr32)
#endif
#if __NVECTORS > 33
		JMP(_intr33)
#endif
#if __NVECTORS > 34
		JMP(_intr34)
#endif
#if __NVECTORS > 35
		JMP(_intr35)
#endif
#if __NVECTORS > 36
		JMP(_intr36)
#endif
#if __NVECTORS > 37
		JMP(_intr37)
#endif
#if __NVECTORS > 38
		JMP(_intr38)
#endif
#if __NVECTORS > 39
		JMP(_intr39)
#endif
#if __NVECTORS > 40
		JMP(_intr40)
#endif
#if __NVECTORS > 41
		JMP(_intr41)
#endif
#if __NVECTORS > 42
		JMP(_intr42)
#endif
#if __NVECTORS > 43
		JMP(_intr43)
#endif
#if __NVECTORS > 44
		JMP(_intr44)
#endif
#if __NVECTORS > 45
		JMP(_intr45)
#endif
#if __NVECTORS > 46
		JMP(_intr46)
#endif
#if __NVECTORS > 47
		JMP(_intr47)
#endif
#if __NVECTORS > 48
		JMP(_intr48)
#endif
#if __NVECTORS > 49
		JMP(_intr49)
#endif
#if __NVECTORS > 50
		JMP(_intr50)
#endif
#if __NVECTORS > 51
		JMP(_intr51)
#endif
#if __NVECTORS > 52
		JMP(_intr52)
#endif
#if __NVECTORS > 53
		JMP(_intr53)
#endif
#if __NVECTORS > 54
		JMP(_intr54)
#endif
#if __NVECTORS > 55
		JMP(_intr55)
#endif

#if __NVECTORS > 0
_intr0:		.weak	_intr0
#endif
#if __NVECTORS > 1
_intr1:		.weak	_intr1
#endif
#if __NVECTORS > 2
_intr2:		.weak	_intr2
#endif
#if __NVECTORS > 3
_intr3:		.weak	_intr3
#endif
#if __NVECTORS > 4
_intr4:		.weak	_intr4
#endif
#if __NVECTORS > 5
_intr5:		.weak	_intr5
#endif
#if __NVECTORS > 6
_intr6:		.weak	_intr6
#endif
#if __NVECTORS > 7
_intr7:		.weak	_intr7
#endif
#if __NVECTORS > 8
_intr8:		.weak	_intr8
#endif
#if __NVECTORS > 9
_intr9:		.weak	_intr9
#endif
#if __NVECTORS > 10
_intr10:	.weak	_intr10
#endif
#if __NVECTORS > 11
_intr11:	.weak	_intr11
#endif
#if __NVECTORS > 12
_intr12:	.weak	_intr12
#endif
#if __NVECTORS > 13
_intr13:	.weak	_intr13
#endif
#if __NVECTORS > 14
_intr14:	.weak	_intr14
#endif
#if __NVECTORS > 15
_intr15:	.weak	_intr15
#endif
#if __NVECTORS > 16
_intr16:	.weak	_intr16
#endif
#if __NVECTORS > 17
_intr17:	.weak	_intr17
#endif
#if __NVECTORS > 18
_intr18:	.weak	_intr18
#endif
#if __NVECTORS > 19
_intr19:	.weak	_intr19
#endif
#if __NVECTORS > 20
_intr20:	.weak	_intr20
#endif
#if __NVECTORS > 21
_intr21:	.weak	_intr21
#endif
#if __NVECTORS > 22
_intr22:	.weak	_intr22
#endif
#if __NVECTORS > 23
_intr23:	.weak	_intr23
#endif
#if __NVECTORS > 24
_intr24:	.weak	_intr24
#endif
#if __NVECTORS > 25
_intr25:	.weak	_intr25
#endif
#if __NVECTORS > 26
_intr26:	.weak	_intr26
#endif
#if __NVECTORS > 27
_intr27:	.weak	_intr27
#endif
#if __NVECTORS > 28
_intr28:	.weak	_intr28
#endif
#if __NVECTORS > 29
_intr29:	.weak	_intr29
#endif
#if __NVECTORS > 30
_intr30:	.weak	_intr30
#endif
#if __NVECTORS > 31
_intr31:	.weak	_intr31
#endif
#if __NVECTORS > 32
_intr32:	.weak	_intr32
#endif
#if __NVECTORS > 33
_intr33:	.weak	_intr33
#endif
#if __NVECTORS > 34
_intr34:	.weak	_intr34
#endif
#if __NVECTORS > 35
_intr35:	.weak	_intr35
#endif
#if __NVECTORS > 36
_intr36:	.weak	_intr36
#endif
#if __NVECTORS > 37
_intr37:	.weak	_intr37
#endif
#if __NVECTORS > 38
_intr38:	.weak	_intr38
#endif
#if __NVECTORS > 39
_intr39:	.weak	_intr39
#endif
#if __NVECTORS > 40
_intr40:	.weak	_intr40
#endif
#if __NVECTORS > 41
_intr41:	.weak	_intr41
#endif
#if __NVECTORS > 42
_intr42:	.weak	_intr42
#endif
#if __NVECTORS > 43
_intr43:	.weak	_intr43
#endif
#if __NVECTORS > 44
_intr44:	.weak	_intr44
#endif
#if __NVECTORS > 45
_intr45:	.weak	_intr45
#endif
#if __NVECTORS > 46
_intr46:	.weak	_intr46
#endif
#if __NVECTORS > 47
_intr47:	.weak	_intr47
#endif
#if __NVECTORS > 48
_intr48:	.weak	_intr48
#endif
#if __NVECTORS > 49
_intr49:	.weak	_intr49
#endif
#if __NVECTORS > 50
_intr50:	.weak	_intr50
#endif
#if __NVECTORS > 51
_intr51:	.weak	_intr51
#endif
#if __NVECTORS > 52
_intr52:	.weak	_intr52
#endif
#if __NVECTORS > 53
_intr53:	.weak	_intr53
#endif
#if __NVECTORS > 54
_intr54:	.weak	_intr54
#endif
#if __NVECTORS > 55
_intr55:	.weak	_intr55
#endif
		reti
