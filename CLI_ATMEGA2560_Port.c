// #############################################################################
// #### Copyright ##############################################################
// #############################################################################

/*
 * Copyright 2024 BaSSeM
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// #############################################################################
// #### Description ############################################################
// #############################################################################

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

#include "Platform.h"

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

#ifndef DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #undef DEBUG
#endif

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#ifdef CLI_ATMEGA2560

// #############################################################################
// #### Include(s) #############################################################
// #############################################################################

    #include "../../CLI_Internal.h"
    #include "CLI_Windows_Port.h"

    #include <stddef.h>
    #include <stdio.h>

// #############################################################################
// #### Private Macro(s) #######################################################
// #############################################################################

// #############################################################################
// #### Private Type(s) ########################################################
// #############################################################################

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

// #############################################################################
// #### Private Variable(s) ####################################################
// #############################################################################

// #############################################################################
// #### Private Method(s) ######################################################
// #############################################################################

CLI_Status_t CLI_IsValid( CLI_t CLI )
{
    CLI_Status_t CLI_Status = CLI_Status_Error;
    do
    {
        CLI_Trace( "%s( CLI=CLI_%d )", __FUNCTION__, CLI );
        switch ( CLI )
        {
            case CLI_1:
                CLI_Status = CLI_Status_Success;
                break;
            default:
                CLI_Error( "Invalid Argument" );
                CLI_Status = CLI_Status_ArgumentInvalid;
                break;
        }
    }
    while ( 0 );
    return CLI_Status;
}

CLI_Status_t CLI_Instance_Write( CLI_Instance_t * CLI_Instance )
{
    CLI_Status_t CLI_Status = CLI_Status_Error;
    do
    {
        CLI_Trace( "%s( Instance=%p )", __FUNCTION__, CLI_Instance );
        if ( ( CLI_Status = CLI_Instance_IsValid( CLI_Instance ) ) != CLI_Status_Success )
        {
            break;
        }
        UART_Status_t UART_Status = UART_Status_Error;
        if ( CLI_Instance->Transmit.Length + UTIL_SizeOfWithoutNull( UTIL_CSI_SelectGraphicRendition( UTIL_CSI_SelectGraphicRenditionReset ) ) >= UTIL_SizeOfWithoutNull( CLI_Instance->Transmit.Content ) )
        {
            if ( ( UART_Status = UART_Write( CLI_Instance->Serial, CLI_Instance->Transmit.Content, CLI_Instance->Transmit.Length ) ) != UART_Status_Success )
            {
                CLI_Status = CLI_Status_Error;
                break;
            }
            CLI_Instance->Transmit.Length = 0;
            CLI_Instance->Transmit.Content[ CLI_Instance->Transmit.Length ] = 0;
        }
        CLI_Instance->Transmit.Length += snprintf( ( char * ) CLI_Instance->Transmit.Content + CLI_Instance->Transmit.Length, UTIL_SizeOfWithoutNull( CLI_Instance->Transmit.Content ) - CLI_Instance->Transmit.Length, "%s", UTIL_CSI_SelectGraphicRendition( UTIL_CSI_SelectGraphicRenditionReset ) );
        if ( ( UART_Status = UART_Write( CLI_Instance->Serial, CLI_Instance->Transmit.Content, CLI_Instance->Transmit.Length ) ) != UART_Status_Success )
        {
            CLI_Status = CLI_Status_Error;
            break;
        }
        CLI_Instance->Transmit.Length = 0;
        CLI_Instance->Transmit.Content[ CLI_Instance->Transmit.Length ] = 0;
        CLI_Status = CLI_Status_Success;
    }
    while ( 0 );
    return CLI_Status;
}

CLI_Status_t CLI_Instance_Read( CLI_Instance_t * CLI_Instance )
{
    CLI_Status_t CLI_Status = CLI_Status_Error;
    do
    {
        CLI_Trace( "%s( Instance=%p )", __FUNCTION__, CLI_Instance );
        if ( ( CLI_Status = CLI_Instance_IsValid( CLI_Instance ) ) != CLI_Status_Success )
        {
            break;
        }
        UART_Status_t UART_Status = UART_Status_Error;
        if ( ( UART_Status = UART_Read( CLI_Instance->Serial, CLI_Instance->Receive.Content + CLI_Instance->Receive.Length, 1 ) ) != UART_Status_Success )
        {
            CLI_Status = CLI_Status_Error;
            break;
        }
        CLI_Instance->Receive.Length += 1;
        if ( CLI_Instance->Receive.Length >= UTIL_SizeOfWithoutNull( CLI_Instance->Receive.Content ) )
        {
            CLI_Error( "Reached MAX supported size" );
            CLI_Instance->Receive.Length = 0;
        }
        CLI_Instance->Receive.Content[ CLI_Instance->Receive.Length ] = 0;
        CLI_Status = CLI_Status_Success;
    }
    while ( 0 );
    return CLI_Status;
}

CLI_Status_t CLI_Instance_Initialize( CLI_Instance_t * CLI_Instance )
{
    CLI_Status_t CLI_Status = CLI_Status_Error;
    do
    {
        CLI_Trace( "%s( Instance=%p )", __FUNCTION__, CLI_Instance );
        if ( ( CLI_Status = CLI_Instance_IsValid( CLI_Instance ) ) != CLI_Status_Success )
        {
            break;
        }
        for ( CLI_t CLI = CLI_1; CLI < CLI_NUMBER_OF_INSTANCES; ++CLI )
        {
            if ( CLI_Instance == &CLI_Context.Instance[ CLI ] )
            {
                CLI_Instance->CLI = CLI;
                break;
            }
        }
        switch ( CLI_Instance->CLI )
        {
            case CLI_1:
                CLI_Instance->Serial = UART_2;
                CLI_Status = CLI_Status_Success;
                break;
            default:
                CLI_Status = CLI_Status_Error;
                break;
        }
        CLI_Instance->Root = NULL;
        CLI_Instance->Active = NULL;

        CLI_Instance->Transmit.Length = 0;
        CLI_Instance->Transmit.Content[ CLI_Instance->Transmit.Length ] = 0;

        CLI_Instance->Receive.Length = 0;
        CLI_Instance->Receive.Content[ CLI_Instance->Receive.Length ] = 0;

        if ( CLI_Status == CLI_Status_Success )
        {
            CLI_Instance->Transmit.Length += snprintf( ( char * ) CLI_Instance->Transmit.Content + CLI_Instance->Transmit.Length, UTIL_SizeOfWithoutNull( CLI_Instance->Transmit.Content ) - CLI_Instance->Transmit.Length, UTIL_CSI_EraseDisplay( UTIL_CSI_EraseDisplayModeEntire ) );
            CLI_Instance->Transmit.Length += snprintf( ( char * ) CLI_Instance->Transmit.Content + CLI_Instance->Transmit.Length, UTIL_SizeOfWithoutNull( CLI_Instance->Transmit.Content ) - CLI_Instance->Transmit.Length, UTIL_CSI_CursorPosition(, ) );
            CLI_Instance->Transmit.Length += snprintf( ( char * ) CLI_Instance->Transmit.Content + CLI_Instance->Transmit.Length, UTIL_SizeOfWithoutNull( CLI_Instance->Transmit.Content ) - CLI_Instance->Transmit.Length, "Platform %sCLI_%d%s", UTIL_CSI_SelectGraphicRendition( UTIL_CSI_SelectGraphicRenditionColorForegroundCyan ), CLI_Instance->CLI, UTIL_CSI_SelectGraphicRendition( UTIL_CSI_SelectGraphicRenditionColorForegroundDefault ) );
            CLI_Instance->Transmit.Length += snprintf( ( char * ) CLI_Instance->Transmit.Content + CLI_Instance->Transmit.Length, UTIL_SizeOfWithoutNull( CLI_Instance->Transmit.Content ) - CLI_Instance->Transmit.Length, "\nVersion %s%s%s", UTIL_CSI_SelectGraphicRendition( UTIL_CSI_SelectGraphicRenditionColorForegroundCyan ), CLI_VERSION, UTIL_CSI_SelectGraphicRendition( UTIL_CSI_SelectGraphicRenditionColorForegroundDefault ) );
            CLI_Instance->Transmit.Length += snprintf( ( char * ) CLI_Instance->Transmit.Content + CLI_Instance->Transmit.Length, UTIL_SizeOfWithoutNull( CLI_Instance->Transmit.Content ) - CLI_Instance->Transmit.Length, "\n" );
            if ( CLI_Instance_Prompt( CLI_Instance ) != CLI_Status_Success )
            {
                // FIXME
            }
        }
    }
    while ( 0 );
    return CLI_Status;
}

CLI_Status_t CLI_Instance_DeInitialize( CLI_Instance_t * CLI_Instance )
{
    CLI_Status_t CLI_Status = CLI_Status_Error;
    do
    {
        CLI_Trace( "%s( Instance=%p )", __FUNCTION__, CLI_Instance );
        if ( ( CLI_Status = CLI_Instance_IsValid( CLI_Instance ) ) != CLI_Status_Success )
        {
            break;
        }
        // TODO De-Allocate All commands' contexts
    }
    while ( 0 );
    return CLI_Status;
}

// #############################################################################
// #### Public Method(s) #######################################################
// #############################################################################

// #############################################################################
// #### Public Variable(s) #####################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#endif /* CLI_ATMEGA2560 */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
