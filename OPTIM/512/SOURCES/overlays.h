;
;	Szilagyi Ladislau
;
;	ladislau_szilagyi@euroqst.ro
;
;	December 2022 - March 2023
;

char InitOverlays(void);
short CallOverlay0(void* Routine, char CalledRomBank);
short CallOverlay1(void* Routine, short Par1, char CalledRomBank);
short CallOverlay1L(void* Routine, long Par1, char CalledRomBank);
short CallOverlay2(void* Routine, short Par1, short Par2, char CalledRomBank);
short CallOverlay2L(void* Routine, long Par1, short Par2, char CalledRomBank);
short CallOverlay2L_(void* Routine, short Par1, long Par2, char CalledRomBank);
short CallOverlay3(void* Routine, short Par1, short Par2, short Par3, char CalledRomBank);
short CallOverlay4(void* Routine, short Par1, short Par2, short Par3, short Par4, char CalledRomBank);
short CallOverlay5(void* Routine, short Par1, short Par2, short Par3, short Par4, short Par5, char CalledRomBank);
void ReturnOverlay0(short ret_val);
void ReturnOverlay0_(short ret_val);
void ReturnOverlay1(short ret_val);
void ReturnOverlay1L(short ret_val);
void ReturnOverlay2(short ret_val);
void ReturnOverlay2L(short ret_val);
void ReturnOverlay3(short ret_val);
void ReturnOverlay4(short ret_val);
void ReturnOverlay5(short ret_val);