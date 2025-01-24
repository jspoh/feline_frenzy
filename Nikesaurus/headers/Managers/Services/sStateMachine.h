/********************************************************************
 * \file   sStateMachine.h
 * \brief  state machine servic
 *
 * \author Bryan Lim Li Cheng
 * \date   January 2025
 * All content © 2025 DigiPen Institute of Technology Singapore, all rights reserved.
 ********************************************************************/
#pragma once


namespace NIKE {
    namespace StateMachine {

        //Temporary Disable DLL Export Warning
        #pragma warning(disable: 4251)

        // Base state class
        struct Istate
        {
            virtual ~Istate() = default;

            virtual void onEnter() = 0;

            virtual void onUpdate() = 0;

            virtual void onExit() = 0;
        };

        // Abstract class to define transitions between states.
        class Itransition {
        public:
            virtual ~Itransition() = default;

            // Determines if the transition to the next state is valid.
            virtual bool isValid() const = 0;

            // Returns the state that should be transitioned to, if valid.
            virtual Istate* getNextState() const = 0;
        };

        class NIKE_API Service {
        public:
            Service() : current_state{ nullptr } {}
            ~Service() = default;

            //Delete Copy Constructor & Copy Assignment
            Service(Service const& copy) = delete;
            void operator=(Service const& copy) = delete;

            /***********************
            * State Managements
            **************************/
            void changeState(Istate* new_state);

            void registerState(Istate* state);

            void removeState(Istate* state);

            /***********************
            * State Managements
            **************************/
            void addTransition(Itransition* transition);

            /***********************
            * FSM Managements
            **************************/
            void init();
            void update();
        private:
            // Pointer to current state
            Istate* current_state;
            // List of states (could be used to keep track of all available states)
            std::vector<Istate*> state_list;  
            // List of transitions
            std::vector<Itransition*> transition_list;  
        };

        //Re-enable DLL Export warning
        #pragma warning(default: 4251)
    }
}