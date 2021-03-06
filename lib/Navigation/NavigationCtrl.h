/**
 * @file NavigationCtrl.h
 * @brief The Navigation Controll class contains the FSM for the Navigation
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - Implementation Navigation Controll FSM - Luca Mazzoleni (luca.mazzoleni@hsr.ch) - 2019-04-23
 * 
 * @date 2019-04-23
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef NAVIGATIONCTRL_H__
#define NAVIGATIONCTRL_H__

#include "LogConfiguration.h"

#include "DriveCtrl.h"

/**
 * @brief Contains the FSM for the Navigation
 * 
 * @image html NavigationCtrl.png width=1000
 * @image html Gametable.png width=1200
 */
class NavigationCtrl {
    //=====PUBLIC====================================================================================
   public:
    /**
    * @brief Enum holds all possible events
    * 
    */
    enum class Event { MoveToTargetPosition,  ///< Ext: Start MoveToTargetPosition
                       PosEndPointReached,    ///< Signal: PosEndPointReached
                       PosTransitReached,     ///< Signal: PosTransitReached
                       PosReached,            ///< Signal: Position reached
                       Error,                 ///< Error occured
                       Resume,                ///< Ext: Resume after Error occured
                       Reset,                 ///<  Ext.: Reset after Error occured
                       NoEvent                ///< No event generated
    };

    /**
    * @brief Enum holds all possible states for the Navigation
    * 
    */
    enum class State { endPoint,      ///< endPoint State
                       toGateway,     ///< toGateway State
                       gateway,       ///< gateway State
                       crossTransit,  ///< crossTransit State
                       toEndPoint,    ///< toEndPoint State
                       resetState,    ///< reset state
                       errorState     ///< error State
    };

    /**
     * @brief Enum holds all possible sectors
     * 
     */
    enum class Sector {
        SorticHandover,                 ///< Sortic - Handover
        SorticToHandover,               ///< Sortic - to Handover
        SorticWaitForGateway,           ///< Sortic - Wait for Gateway
        SorticGateway,                  ///< Sortic - Gateway
        TransitWaitForGatewaySortic,    ///< Transit - Wait for Gateway Sortic
        TransitToSortic,                ///< Transit - to Sortic
        TransitToTransfer,              ///< Transit - to Transfer
        Parking,                        ///< Parking (not used atm)
        TransitWaitForGatewayTransfer,  ///< Transit - Wait for Gateway Transfer
        TransferGateway,                ///< Transfer - Gateway
        TransferWaitForGateway,         ///< Transfer - wait for Gateway
        TransferToHandover,             ///< Transfer - to Handover
        TransferHandover,               ///< Transfer - Handover
        error                           ///< error for unknown sector
    };

    /**
     * @brief Construct a new Navigation Ctrl object
     * and initailize the currentState with endPoint state
     * 
     */
    NavigationCtrl();

    /**
     * @brief Calls the do-function of the active state and hence generates Events
     * 
     */
    void loop();

    /**
     * @brief Procceses the current Event and calls the do-function of the active state
     * 
     * @param currentEvent - Event
     */
    void loop(Event currentEvent);

    /**
     * @brief Get the current State
     * 
     * @return State - current State
     */
    const State getcurrentState();

    /**
     * @brief Get the current Sector
     * 
     * @return const Sector - current Sector
     */
    const Sector getcurrentSector();

    /**
     * @brief Get the current Line
     * 
     * @return const int - current line
     */
    const int getcurrentLine();

    /**
     * @brief Set the Target Position object
     * 
     * @param sector - TargetSector ( HandoverSortic or HandoverTransfer)
     * @param line - TargetLine
     */
    void setTargetPosition(Sector sector, const int line);

    /**
     * @brief Set the actual Position object
     * 
     * @param sector - actualSector ( HandoverSortic or HandoverTransfer)
     * @param line - actualLine
     */
    void setActualPosition(Sector sector, const int line);

    /**
     * @brief give Token to access the Gateway
     * 
     */
    void giveToken();

    /**
     * @brief Decodes the Sector-Enum and returns a description
     * 
     * @param sector - enum Sector
     * @return String - Sector as String
     */
    String decodeSector(Sector sector);

    /**
     * @brief Decodes a sectro String and returns the sector
     * 
     * @param sector - Sector as String
     * @return NavigationCtrl::Sector -  enum Sector
     */
    NavigationCtrl::Sector decodeSector(String sector);

    //=====PRIVATE====================================================================================
   private:
    State lastStateBevorError;        ///< holds the last state of the FSM so it's possible to resume after error
    State currentState;               ///< holds the current state of the FSM
    Event currentEvent;               ///< holds the current event of the FSM
    int pCurrentSubState = 0;         ///< Counter for the Current Substate
    int pLastSubStateBevorError = 0;  ///< Holds the last Substate befor an Error occured
    int pSubStateLoopInc = 0;         ///< Counter so you can call the same substatefunction multiple times

    /**
     * @brief Holds the actual information about the vehicleposition
     * 
     */
    struct ActualPos {
        Sector startSector = Sector::SorticHandover;  ///< Startpoint from current path
        Sector lastSector;                            ///< last sector
        Sector sector = startSector;                  ///< actual sector
        int line = 1;                                 ///< actual line
        bool token = false;                           ///< token for gateway accees
    } pActual;

    /**
     * @brief Holds the information about the vehicle target
     * 
     */
    struct TargetPos {
        Sector sector;  ///< TargetSector
        int line;       ///< TargetLine
    } pTarget;

    bool tranistonce = true;            ///< prevents an loop between transit and gateway
    DriveCtrl::Event pLastGatewayTurn;  ///< Variable which holds the last made turn in gateway

    /**
     * 
     * @brief Functionpointer to call the current states do-function
     * 
     * https://stackoverfendPoint.com/questions/1485983/calling-c-class-methods-via-a-function-pointer
     */
    Event (NavigationCtrl::*doActionFPtr)(void) = nullptr;

    DriveCtrl pDriveCtrl;  ///< Navigation Object

    //=====PrivateFunctions=========================================================================
    /**
     * @brief changes the state of the FSM based on the event
     * 
     * @param e - Event
     */
    void process(Event e);
    //=====StateFunctions=====
    //=====endPoint==========================================================
    /**
     * @brief executes the entry action of the endPoint state.
     * 
     * Update actual sector and startsector with taregt sector.
     */
    void entryAction_endPoint();

    /**
     * @brief executes the main action of the endPoint state.
     * This is an Idel-state ->NoEvent generated
     * @return NavigationCtrl::Event - generated Event
     */
    NavigationCtrl::Event doAction_endPoint();

    /**
     * @brief executes the exit action of the endPoint state.
     * 
     */
    void exitAction_endPoint();

    //=====toGateway==========================================================
    /**
     * @brief executes the entry action of the toGateway state.
     * 
     */
    void entryAction_toGateway();

    /**
     * @brief executes the main action of the toGateway state.
     * 
     * @image html NavigationCtrlSubtoGateway.png width=200
     * 
     * - 0 Drive backwards
     * - 10 turn around
     * - 20 drive forward once and return Event::PosReached
     * 
     * @return NavigationCtrl::Event - generated Event
     */
    NavigationCtrl::Event doAction_toGateway();

    /**
     * @brief executes the exit action of the toGateway state.
     */
    void exitAction_toGateway();

    //=====gateway==========================================================
    /**
     * @brief executes the entry action of the gateway state.
     * 
     */
    void entryAction_gateway();

    /**
     * @brief executes the main action of the gateway state.
     * 
     * @image html NavigationCtrlSubGateway.png width=300
     * 
     * - 0 Wait for Token
     * - 10 Drive forward once
     * - 20  turn left/right depending on target and current orientation or go straight to (50)
     * - 30 drive forward n times depending on actual line and targetline
     * - 40 turn left/right depending on target and current orientation
     * - 50 drive forward twice.
     * - if startsector = target.sector
     *      - return Event::PosEndPointReached
     * - else
     *      - return Event::PosTransitReached
     * 
     * @return NavigationCtrl::Event - generated Event
     */
    NavigationCtrl::Event doAction_gateway();

    /**
     * @brief executes the exit action of the gateway state.
     * 
     */
    void exitAction_gateway();

    //=====crossTransit==========================================================
    /**
     * @brief executes the entry action of the crossTransit  state.
     * 
     */
    void entryAction_crossTransit();

    /**
     * @brief executes the main action of the crossTransit state.
     * 
     * drive forward three times and return Event::PosReached
     * 
     * @return NavigationCtrl::Event - generated Event
     */
    NavigationCtrl::Event doAction_crossTransit();

    /**
     * @brief executes the exit action of the crossTransit state.
     */
    void exitAction_crossTransit();

    //=====toEndPoint==========================================================
    /**
     * @brief executes the entry action of the toEndPoint  state.
     * 
     */
    void entryAction_toEndPoint();

    /**
     * @brief executes the main action of the toEndPoint state.
     * 
     * drive forward once and return Event::PosEndPointReached
     * @return NavigationCtrl::Event - generated Event
     */
    NavigationCtrl::Event doAction_toEndPoint();

    /**
     * @brief executes the exit action of the toEndPoint state.
     */
    void exitAction_toEndPoint();

    //==errorState==========================================================
    /**
     * @brief entry action of the errorState state.
     * 
     */
    void entryAction_errorState();

    /**
     * @brief main action of the errorState state.
     * 
     *  @return NavigationCtrl::Event - generated Event
     */
    NavigationCtrl::Event doAction_errorState();

    /**
     * @brief exit action of the errorState state.
     * 
     */
    void exitAction_errorState();

    //==resetState==========================================================
    /**
     * @brief entry action of the resetState
     * 
     */
    void entryAction_resetState();

    /**
     * @brief main action of the resetState
     * 
     *  @return NavigationCtrl::Event - generated Event
     */
    NavigationCtrl::Event doAction_resetState();

    /**
     * @brief exit action of the resetState
     * 
     */
    void exitAction_resetState();
    //============================================================================
    //==Aux-Function==============================================================
    /**
     * @brief Decodes the State-Enum and returns a description
     * 
     * @param state - enum State
     * @return String - State as String
     */
    String decodeState(State state);

    /**
     * @brief Decodes the Event-Enum and returns a description
     * 
     * @param event - enum Event
     * @return String - Event as String
     */
    String decodeEvent(Event event);
};
#endif
