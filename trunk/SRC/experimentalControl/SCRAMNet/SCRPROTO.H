    /* 
    -----------------------------------------------------------------------------
    SCRPROTO.H - This file defines function common to all SCRAMNet WINDOWS DLLs. 
    -----------------------------------------------------------------------------
    */
    #include <stdio.h> // FILE

#ifndef _WINNT
#define _WINNT
#endif

    #define Long_mode 0
    #define Word_mode 1
    #define Byte_mode 2
       
    /* Handle C++ name mangling. */
    #ifdef _cplusplus
    extern "C" {
    #endif
    
    /* Define interrupt object name. */
    #define SCR_INTERRUPT_OBJECT "SCRAMNet_Int_Object"
    
    DLL_DEF SCR_INT CALL_CONV scr_long_mem_exist( SCR_LONG_PTR mem_ptr );
    // Determines if long word memory location is valid.
    
    DLL_DEF SCR_INT CALL_CONV scr_short_mem_exist( SCR_SHORT_PTR mem_ptr );
    // Determines if short word memory location is valid.
    
    DLL_DEF SCR_INT CALL_CONV scr_byte_mem_exist( SCR_BYTE_PTR mem_ptr );
    // Determines if byte memory location is valid.
    
    DLL_DEF SCR_INT CALL_CONV sp_scram_init( void );
    // Initialize the board by reading values from ini file, mapping SCRAMNet 
    // Registers, mapping SCRAMNet memory, and setting node ID and timeout 
    // values equal to the values specifed in the Registry.
    
    DLL_DEF SCR_LONG_PTR CALL_CONV get_base_mem( void );
    // Return a virtual pointer to a block of memory mapped into the SCRAMNet 
    // cards physical address space.
    
    DLL_DEF BOOL CALL_CONV SetScrTransactionType( BYTE mode );
    // This function configures the SCRAMNet Card to swap the values read from 
    // memory according the paramater passed to the function.
    // **************************************************************************
    // * Paramater * Swap mode      * data bits translation                     *
    // **************************************************************************
    // *           *                * D(31:24) * D(23:16) * D(15:8)  *  D(7:0)  *
    // **************************************************************************
    // * Long_mode * no swap 32-bit * Q(31:24) * Q(23:16) * Q(15:8)  *  Q(7:0)  *
    // * Word_mode * 16 bit         * Q(15:8)  * Q(7:0)   * Q(31:24) * Q(23:16) *
    // * Byte_mode * 8 bit          * Q(7:0)   * Q(15:8)  * Q(23:16) * Q(31:24) *
    // **************************************************************************
    
    DLL_DEF BYTE CALL_CONV GetScrTransactionType( void );
    // this function return a byte representing the current swaping mode.
    
    DLL_DEF USHORT CALL_CONV scr_csr_read( char csr_number );
    // Returns contents of CSR register specifed as a parameter.
    
    DLL_DEF void CALL_CONV scr_csr_write( char csr_number, USHORT regval );
    // Write given value to the CSR register specifed as a parameter.
    
    DLL_DEF DWORD CALL_CONV ReadSCRLong( DWORD offset, DWORDPTR lnValue);  
    // Returns long word value read from scramnet card at the given offset. 
    // offset specifed as number of long words from base address to desired value.
    
    DLL_DEF DWORD CALL_CONV WriteSCRLong( DWORD offset, DWORD lnValue);
    // returns long word writen to SCRAMNet card at the given offset. 
    // offset specifed as number of long words from base address to desired value.
    
    DLL_DEF WORD CALL_CONV ReadSCRWord( DWORD offset, WORDPTR nValue);
    // returns word value read from scramnet card at the given offset. 
    // offset specifed as number of words from base address to desired value.
    
    DLL_DEF WORD CALL_CONV WriteSCRWord( DWORD offset, WORD nValue);
    // returns word writen to SCRAMNet card at the given offset. 
    // offset specifed as number of words from base address to desired value.
    
    DLL_DEF BYTE CALL_CONV ReadSCRByte( DWORD offset, BYTEPTR nValue);
    // returns byte value read from scramnet card at the given offset. 
    // offset specifed as number of bytes from base address to desired value.
    
    DLL_DEF BYTE CALL_CONV WriteSCRByte( DWORD offset, BYTE lnValue);
    // returns byte writen to SCRAMNet card at the given offset. 
    // offset specifed as number of bytes from base address to desired value.
    
    DLL_DEF DWORD CALL_CONV get_scr_phy_csr_addr( void ); 
    // Returns physical memory address of the SCRAMNet CSR base resgister.
    
    DLL_DEF DWORD CALL_CONV get_scr_phy_mem_addr( void );
    // Returns physical memory address of SCRAMNet memory base address.
    
    DLL_DEF BYTE CALL_CONV get_scr_node_id(  void );
    // Returns a byte value in range 0 to 255 for SCRAMNet node id.
    
    DLL_DEF BYTE CALL_CONV get_scr_time_out( void );
    // Returns a byte value in range 0 to 255 for SCRAMNet Timeout value
    
    DLL_DEF DWORD CALL_CONV get_scr_user_mem_size( void );
    // Return an unsigned long value equal to the memory size in bytes 
    // specifed in the SCRAMNET.INI file.
    // Note: ini file specifies memory size in Kbytes. Function returns bytes.
    
    DLL_DEF DWORD CALL_CONV sp_mem_size( void );
    // Return an unsigned long values equal to the number of bytes of 
    // of SCRAMNet memmory as represented in csr 8.
                         
    DLL_DEF void CALL_CONV scr_board_status( HWND hwndParent);
    // displays a message window with SCRAMNet configuration information in it.
    
    DLL_DEF void CALL_CONV scr_mclr_mm( SCR_INT arg );
    // Clear all of memory or ACR bits depending on parameter passed in arg.
    // To clear memory use defined value 'MEM' as the parameter or 
    // to clear ACR  use defined value 'ACR'.
    
    DLL_DEF ULONG CALL_CONV scr_probe_mm( void* addr_ptr, unsigned short md_flg );
    // Determines validity of specified address.
    
    DLL_DEF DWORD CALL_CONV sw_mem_addr( void );
    // Returns physical memory address of SCRAMNet memory base address.
    
    DLL_DEF DWORD CALL_CONV sw_reg_addr( void );
    // Returns physical memory address of SCRAMNet memory base address.
    
    DLL_DEF DWORD CALL_CONV sw_user_size( void );
    // Return an unsigned long value equal to the memory size in bytes 
    // specifed in the Registry.
    // Note: Registry specifies memory size in Kbytes. Function returns bytes.
    
    DLL_DEF SCR_INT CALL_CONV sw_cfg_fill( void *ConfigStruct );
    // Fills DOS/unix configuration structure
    
    DLL_DEF VOID CALL_CONV scr_id_mm( char *nID, char *nCnt );
    // Fills byte value in range 0 to 255 for SCRAMNet node id and total nodes on net
    
    DLL_DEF SCR_INT CALL_CONV sw_net_to( void );
    // Returns a byte value in range 0 to 255 for SCRAMNet Timeout value
    
    DLL_DEF BOOL CALL_CONV sw_set_size( DWORD lnNewSize ); 
    // Write new "user-specified" memory size to Registry (this amount will be mapped
    // versus the amount on the card).
    
    DLL_DEF BYTE CALL_CONV sw_get_int( void ); 
    // Read interrupt number from Registry.
    
    DLL_DEF BOOL CALL_CONV sw_set_int( BYTE nIntNum );
    // Write new interrupt number to Registry.
    
    DLL_DEF SCR_INT CALL_CONV scr_mem_mm( SCR_INT arg );
    // Maps/unmaps SCRAMNet memory
    
    DLL_DEF SCR_INT CALL_CONV scr_reg_mm( SCR_INT arg );
    // Maps/unmaps SCRAMNet memory
    
    DLL_DEF SCR_INT CALL_CONV scr_brd_select( SCR_INT brd );
    // Changes control of SCRAMNet boards
    
    // DMA read from SCRAMNet memory to user buffer
    DLL_DEF SCR_INT CALL_CONV scr_dma_read( PVOID user_addr, ULONG scr_offset, ULONG num_bytes );
    
    // DMA write from user buffer to SCRAMNet memory
    DLL_DEF SCR_INT CALL_CONV scr_dma_write( PVOID user_addr, ULONG scr_offset, ULONG num_bytes );
    
    DLL_DEF SCR_INT CALL_CONV sp_cfg_read( SCR_INT board_number );
    // Function unnecessary in Windows NT (here for compatibility).
    
    DLL_DEF unsigned  char CALL_CONV scr_acr_read( unsigned long mem_loc );
    // Reads specified ACR register
    
    DLL_DEF void CALL_CONV scr_acr_write( unsigned long mem_loc, unsigned char acr_val );
    // Writes to a specified ACR register
    
    DLL_DEF void CALL_CONV scr_error_mm( FILE * ofd, unsigned short int tmp_csr );
    // Provides textual description of errors
    
    DLL_DEF SCR_INT CALL_CONV scr_fswin_mm( void );
    // Checks the state of the "Fiber Optic Bypass Not Connected" bit in CSR1
    
    DLL_DEF void CALL_CONV scr_fifo_mm( SCR_INT cmd, struct rd_fifo* f_ptr );
    // Reset shared memory, interrupt, and Tx/Tx FIFO or read CSR1
    
    DLL_DEF SCR_INT CALL_CONV scr_load_mm( char *str_ptr, SCR_INT cmd );
    // Loads user-supplied configuration file
    
    DLL_DEF SCR_INT CALL_CONV scr_read_int_fifo( unsigned long int *fifo_value );
    // Reads interrupt FIFO.
    
    DLL_DEF void CALL_CONV scr_rw_mm( struct rw_scr *op ); 
    // Reads/writes specified CSR.
    
    DLL_DEF SCR_INT CALL_CONV scr_save_mm( char *strptr, SCR_INT cmd ); 
    // Saves memory or ACR contents to file.
    
    DLL_DEF void CALL_CONV scr_smem_mm( SCR_INT arg, unsigned long value );
    // Set memory or ACR to value.
    
    DLL_DEF SCR_INT CALL_CONV sp_bist_rd( SCR_INT *bitstream );
    // Reads built-in-self-test info from Plus/LX cards.
    
    DLL_DEF SCR_INT CALL_CONV sp_msg_life( SCR_INT hops );
    // Set pre-age of network messages.
    
    DLL_DEF SCR_INT CALL_CONV sp_net_to( unsigned short timeout );
    // Set network timeout.
    
    DLL_DEF SCR_INT CALL_CONV sp_protocol( SCR_INT ProtocolMode );
    // Set network protocol.
    
    DLL_DEF SCR_INT CALL_CONV sp_rx_id( unsigned char NewID ); 
    // Set node receive ID.
    
    DLL_DEF SCR_INT CALL_CONV sp_set_cntr( SCR_INT mode );
    // Set general purpose counter.
    
    DLL_DEF SCR_INT CALL_CONV sp_set_sm_addr( unsigned long addr );
    // Set physical memory address.
    
    DLL_DEF SCR_INT CALL_CONV sp_set_vp( SCR_INT request, SCR_INT pagenumber );
    // Set virtual page number.
    
    DLL_DEF SCR_INT CALL_CONV sp_txrx_id( unsigned char NewID ); 
    // Set transmit/receive ID.
    
    DLL_DEF void CALL_CONV scr_reset_mm( void );
    // Initializes node to zero condition. 
    
    DLL_DEF SCR_INT CALL_CONV sp_plus_find( void );
    // Determines card type. 
    
    DLL_DEF BOOL CALL_CONV sp_stm_mm( BYTE mode );
    // Sets transaction type.
    
    DLL_DEF BYTE CALL_CONV sp_gtm_mm( void );
    // Gets transaction type.
    
    DLL_DEF void CALL_CONV scr_bswp_mm( void );
    // Sets byteswap mode (invalid for PCI).
    
    DLL_DEF void CALL_CONV scr_dfltr_mm( SCR_INT arg );
    // Data filtering control switch.
    
    DLL_DEF void CALL_CONV scr_lnk_mm( SCR_INT cmd );
    // Link SCRAMNet node to network.
    
    DLL_DEF void CALL_CONV scr_wml_mm( SCR_INT arg );
    // Set update mode.
    
    DLL_DEF void CALL_CONV scr_acr_mm( SCR_INT enab_fl );
    // Enable ACRs.
    
    DLL_DEF void CALL_CONV scr_int_mm( SCR_INT cmd, SCR_INT arg );
    // Set interrupt mode.
    
    DLL_DEF BOOL CALL_CONV sw_int_connect( void );
    // Connect interrupt processing.
    
    DLL_DEF BOOL CALL_CONV sw_int_disconnect( void );
    // Disconnect interrupt processing.
    
    DLL_DEF BOOL CALL_CONV sw_int_snapicon( void );
    // Connect interrupt processing.
  
    DLL_DEF BOOL CALL_CONV sw_int_snapidis( void );
    // Disconnect interrupt processing.
  
    DLL_DEF SCR_INT CALL_CONV scr_delay(UINT delay);
    // Provides a general delay in ms
    
	DLL_DEF BOOL CALL_CONV sw_int_flush( void );
    // Connect interrupt processing.

	DLL_DEF BOOL CALL_CONV sw_set_debugLevel( UINT32 level ); 
	// Alter debuglevel flag used to control which debugPrint

	DLL_DEF UINT32 CALL_CONV sw_get_debugLevel( void ); 
	// get debuglevel flag used to control which debugPrint
    
    #ifdef _cplusplus
    }

    #endif
       
    
