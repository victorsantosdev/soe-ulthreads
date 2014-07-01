#define uint8_t byte

//save context
#define SAVE_CONTEXT()
asm volatile (
"push __tmp_reg__         \n\t" /*__tmp_reg__ symbol for R0*/
"in __tmp_reg__, __SREG__ \n\t"
"cli	   		          \n\t"
"push __tmp_reg__   	  \n\t"
"push __zero_reg__    	  \n\t" /*__zero_reg__ symbol for R1*/
"clr  __zero_reg__        \n\t"
"push r2         \n\t"
"push r3         \n\t"
"push r4         \n\t"
"push r5         \n\t"
"push r6         \n\t"
"push r7         \n\t"
"push r8         \n\t"
"push r9         \n\t"
"push r10        \n\t"
"push r11        \n\t"
"push r12        \n\t"
"push r13  		 \n\t"
"push r14        \n\t"
"push r15        \n\t"
"push r15        \n\t"
"push r16        \n\t"
"push r17        \n\t"
"push r18        \n\t"
"push r19        \n\t"
"push r20        \n\t"
"push r21        \n\t"
"push r22        \n\t"
"push r23        \n\t"
"push r24        \n\t"
"push r25        \n\t"
"push r26        \n\t"
"push r27        \n\t"
"push r28        \n\t"
"push r29        \n\t"
"push r30        \n\t"
"push r31        \n\t"
"lds r26, context         \n\t" /* Put *context in r26 (low byte of X) */
"lds r27, context + 1     \n\t" /* Put *(context+1) in r27 (hight byte of X) */
"in r24, __SP_L__	      \n\t" /* Put SPL content in r24 */ 
"in r25, __SP_H__	      \n\t" /* Put SPH content in r25 */
"adiw r24, 0x01 	      \n\t" /* Add 1 to r24 (Saved SPL) */
"st x+, r24       \n\t" /* Store r24 content at X */
"st x+, r25       \n\t" /* Store r25 content at X+1 */
);


//restore context
#define RESTORE_CONTEXT()
asm volatile (
"lds r26, context          \n\t" /* Load *context in r26 (XL)*/
"lds r27, context + 1      \n\t" /* Load *(context+1) in r27 (XH)*/
"ld r28, x+       		   \n\t" /* Load *X in r28 and X=X+1 */
"ld r29, x+       		   \n\t" /* Load *X in r29 */
"sbiw r28, 0x01    		   \n\t" /* Remove 1 to r28 */
"out __SP_L__, r28         \n\t" /* Put r28 content in SPL */
"out __SP_H__, r29         \n\t" /* Put r29 content in SPH */
"pop r31         \n\t"
"pop r30         \n\t"
"pop r29         \n\t"
"pop r28         \n\t"
"pop r27         \n\t"
"pop r26         \n\t"
"pop r25         \n\t"
"pop r24         \n\t"
"pop r23         \n\t"
"pop r22         \n\t"
"pop r21         \n\t"
"pop r20         \n\t"
"pop r19         \n\t"
"pop r18         \n\t"
"pop r17         \n\t"
"pop r16         \n\t"
"pop r15         \n\t"
"pop r14         \n\t"
"pop r13         \n\t"
"pop r12         \n\t"
"pop r11         \n\t"
"pop r10         \n\t"
"pop r9         \n\t"
"pop r8         \n\t"
"pop r7         \n\t"
"pop r6         \n\t"
"pop r5         \n\t"
"pop r4         \n\t"
"pop r3         \n\t"
"pop r2         \n\t"
"pop __zero_reg__             \n\t"
"pop __tmp_reg__              \n\t"
"out __SREG__ __tmp_reg__     \n\t"
"pop __tmp_reg__              \n\t"
);

__attribute__ ((naked)) void switchContext( byte * old_context, byte * new_context)) {
	DEBUG_PRINTF("SRV_trap__switchContext \n");
	DEBUG_PRINTF("Saving old_context : %x \n", old_context);
	// Make the global variable context
	// point on the current stack pointer
	// to use it the assembly
	// SAVE_CONTEXT macro.
	context = old_context;
	// Save current context
	SAVE_CONTEXT();
	// restore the new context only if it exists
	if( new_context != 0 ) {
		DEBUG_PRINTF("Restoring new_context : %x \n", new_context);
		// Make the global variable context
		// point on the new stack pointer
		// to use it the assembly
		// RESTORE_CONTEXT macro.
		context = new_context;
		// Set the new context
		RESTORE_CONTEXT();
	}
	else {
		DEBUG_PRINTF("Trying to load a non existing context ? \n");
	}
	// naked function. need manual ret
	__asm__ __volatile__ ( "ret" );
}

/*stack init*/

void SRV_trap__initContext(uint8_t* virtcontext, any code_entry, uint8_t stackbase[], jint stacksize, any code_entry_argument, jboolean user) {
	jushort temp_address;
	DEBUG_PRINTF("SRV_trap__initContext \n");
	// Let’s start to write at the begining of the stack
	// which is the end of the memory region.
	temp_address = (jushort) code_entry;
	// Write PC-1 on the stack
	stackbase[--stacksize] = (jbyte) ( temp_address & (jushort) 0x00ff );
	// Write PC-2 on the stack
	temp_address >>= 8;
	stackbase[--stacksize] = (jbyte) ( temp_address & (jushort) 0x00ff );

	//warning I can’t find out why code_entry wouldn’t be more than exp(2,16).
	// Seems to work anyway, but wasn’t tested a lot.
	stackbase[--stacksize] = (jbyte) ( 0 ); /* R0 */
	// Start tasks with interrupts enabled.
	stackbase[--stacksize] = (jbyte) ( 0x80 ); /* SREG */
	// The compiler expects R1 to be 0.
	stackbase[--stacksize] = (jbyte) ( 0x0 ); /* R1 */
	stackbase[--stacksize] = (jbyte) ( 2 ); /* R2 */
	stackbase[--stacksize] = (jbyte) ( 3 ); /* R3 */
	stackbase[--stacksize] = (jbyte) ( 4 ); /* R4 */
	stackbase[--stacksize] = (jbyte) ( 5 ); /* R5 */
	stackbase[--stacksize] = (jbyte) ( 6 ); /* R6 */
	stackbase[--stacksize] = (jbyte) ( 7 ); /* R7 */
	stackbase[--stacksize] = (jbyte) ( 8 ); /* R8 */
	stackbase[--stacksize] = (jbyte) ( 9 ); /* R9 */
	stackbase[--stacksize] = (jbyte) ( 10 ); /* R10 */
	stackbase[--stacksize] = (jbyte) ( 11 ); /* R11 */
	stackbase[--stacksize] = (jbyte) ( 12 ); /* R12 */
	stackbase[--stacksize] = (jbyte) ( 13 ); /* R13 */
	stackbase[--stacksize] = (jbyte) ( 14 ); /* R14 */
	stackbase[--stacksize] = (jbyte) ( 15 ); /* R15 */
	stackbase[--stacksize] = (jbyte) ( 16 ); /* R16 */
	stackbase[--stacksize] = (jbyte) ( 17 ); /* R17 */
	stackbase[--stacksize] = (jbyte) ( 18 ); /* R18 */
	stackbase[--stacksize] = (jbyte) ( 19 ); /* R19 */
	stackbase[--stacksize] = (jbyte) ( 20 ); /* R20 */
	stackbase[--stacksize] = (jbyte) ( 21 ); /* R21 */
	stackbase[--stacksize] = (jbyte) ( 22 ); /* R22 */
	stackbase[--stacksize] = (jbyte) ( 23 ); /* R23 */
	stackbase[--stacksize] = (jbyte) ( 24 ); /* R24 */
	stackbase[--stacksize] = (jbyte) ( 25 ); /* R25 */
	stackbase[--stacksize] = (jbyte) ( 26 ); /* R26 X */
	stackbase[--stacksize] = (jbyte) ( 27 ); /* R27 */
	stackbase[--stacksize] = (jbyte) ( 28 ); /* R28 Y */
	stackbase[--stacksize] = (jbyte) ( 29 ); /* R29 */
	stackbase[--stacksize] = (jbyte) ( 30 ); /* R30 Z */
	stackbase[--stacksize] = (jbyte) ( 31 ); /* R31 */

	temp_address = (jushort) stackbase + stacksize;
	*virtcontext = (jbyte) ( temp_address & (jushort) 0x00ff );
	temp_address >>= 8;
	*(virtcontext+1) = (jbyte) ( temp_address & (jushort) 0x00ff );
}

//signal handler

/* Quando a interupção ocorre, o compilador GCC já salva automaticamente
todos os registradores modificados pela ISR. Quando o contexto é trocado, 
todo o context da tarefa PRECISA ser salvo. 
O atributo naked previne alguns registradores sejam salvos duas vezes neste processo 
Neste caso somente o program counter é salvo.*/

void SIG_OUTPUT_COMPARE1A(void) __attribute__ ( (signal, naked) );

void SIG_OUTPUT_COMPARE1A(void) {
	yield();
}

volatile long long tick = 0;

void cfgTimer1 (void) {
	/*configura o timer1 para 1 tick a cada 1ms */
	TCCR1A |= 0;
	TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); //CTC enabled, prescaler 1024

	TCTN1 = 0; //initialize counter 
	OCR1A = 156; //contagem para 1ms de tick
	TIMSK |= (1 << OCIE1A); // enable compare interrupt

	sei(); // enable global interrupts
} 

ISR (TIMER1_COMPA_vect)
{
    tick++;
}