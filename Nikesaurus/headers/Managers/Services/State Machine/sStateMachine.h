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

        // Forward declaration to fix errors
        class Istate;


        // Base transition class
        class Itransition {
        public:
            // Determines if the transition to the next state is valid
            virtual bool isValid(Entity::Type& entity) const = 0;

            // Returns the state that should be transitioned to.
            virtual std::shared_ptr<Istate> getNextState() const = 0;
        };

        // Base state class
        class Istate
        {
        public:
            virtual ~Istate() = default;

            virtual void onEnter(Entity::Type& entity) = 0;

            virtual void onUpdate(Entity::Type& entity) = 0;

            virtual void onExit(Entity::Type& entity) = 0;

            /***********************
            * Transitions handling
            ************************/
            std::unordered_map<std::string, std::shared_ptr<Itransition>> const& getTransitions();
            void addTransition(const std::string& transition_id, std::shared_ptr<Itransition> transition);


        private:
            // List of transitions
            std::unordered_map<std::string, std::shared_ptr<Itransition>> transitions;
        };

        class NIKE_API Service {
        public:
            Service() = default;
            ~Service() = default;

            //Delete Copy Constructor & Copy Assignment
            Service(Service const& copy) = delete;
            void operator=(Service const& copy) = delete;

            /***********************
            * State Managements
            **************************/
            void changeState(std::shared_ptr<Istate> new_state, Entity::Type& entity);

            void registerState(const std::string& state_id, std::shared_ptr<Istate> state);

            void removeState(const std::string& state_id);


            /***********************
            * FSM Managements
            **************************/

            template <typename T>
            std::shared_ptr<T> getStateByID(const std::string& state_id) const {
                auto it = state_map.find(state_id);
                if (it != state_map.end()) {
                    // Cast the shared pointer to the correct type T
                    return std::dynamic_pointer_cast<T>(it->second);
                }
                return nullptr;
            }

            std::shared_ptr<Istate> getStateByID(const std::string& state_id) const;

            void init();
            void update(Entity::Type& entity);

        private:
            // Shared pointer to current state
            // FSM class will hold ownsership of the entity's current state
            std::shared_ptr<Istate> current_state;
            // List of states map
            std::unordered_map<std::string, std::shared_ptr<Istate>> state_map;
        };

        //Re-enable DLL Export warning
        #pragma warning(default: 4251)
    }
}