#include "variable_manager.h"
#include <vector>
#include <inttypes.h>
#include "logging.h"
#include "variable/string_variable.h"
#include <pthread.h>

#define TOKEN_VARIABLE_START '{'
#define TOKEN_VARIABLE_END   '}'
#define TOKEN_VARIABLE_SPACE ' '

#define TOKEN_TYPE_STRING 0
#define TOKEN_TYPE_VARIABLE 1

VariableManager::VariableManager() {
    mutex = PTHREAD_MUTEX_INITIALIZER;
    int mutexInitResult = pthread_mutex_init(&mutex, NULL);
    if (mutexInitResult) {
        ALOGE("Error while initializing mutex!");
        return;
    }
}

VariableManager::~VariableManager() {
    pthread_mutex_destroy(&mutex);
}

int VariableManager::addVariable(const string & key,Variable * var) {
    pthread_mutex_lock(&mutex);

    if (hasElement(key)) {
        VariableWrapper * managedElement = Manager<VariableWrapper,string>::getElement(key);
        managedElement->set(var);
    } else {
        Manager<VariableWrapper,string>::addElement(key, new VariableWrapper(var));
    }

    pthread_mutex_unlock(&mutex);

    return 0;
}

VariableCollection * VariableManager::buildString(const string & split) {
    VariableCollection * collection = new VariableCollection();
    char currentSegment[255] = {0};
    uint8_t currentCharacter = 0;
    uint8_t currentTokenType = TOKEN_TYPE_STRING;

    for (int32_t index = 0; index < split.size() ; index++ ) {
        uint8_t newTokenType = currentTokenType;
        switch(split[index]) {
            case TOKEN_VARIABLE_START:
                if (currentTokenType == TOKEN_TYPE_VARIABLE) {
                    ERROR_LOG("string: %s, at index: %d is improperly formatted", split.c_str(), index);
                }
                newTokenType = TOKEN_TYPE_VARIABLE;
                break;
            case TOKEN_VARIABLE_END:
                if (currentTokenType != TOKEN_TYPE_VARIABLE) {
                    ERROR_LOG("string: %s, at index: %d is improperly formatted", split.c_str(), index);
                }

                newTokenType = TOKEN_TYPE_STRING;
                break;
            default:
                currentSegment[currentCharacter++] = split[index];
                break;
        }

        if ( newTokenType != currentTokenType || index == split.size() - 1) {
            if (currentCharacter!= 0) {
                string tokenName(currentSegment);
                memset(currentSegment,0x00,255);
                currentCharacter = 0;

                if (currentTokenType == TOKEN_TYPE_VARIABLE) {
                    const VariableWrapper * var = getElement(tokenName);
                    if (var == NULL) {
                        collection->addVariable(new VariableWrapper(new StringVariable("(NULL)"),true));
                    } else {
                        collection->addVariable(var);
                    }

                } else {
                    collection->addVariable(new VariableWrapper(new StringVariable(tokenName),true));
                }
            }

            currentTokenType = newTokenType;
        }
    }

    return collection;
}

string VariableManager::getVariable(const char * key) {
    const VariableWrapper * var = getElement(key);
    if (var == NULL) {
        return "";
    } else {
        return var->get()->value();
    }
}

VariableWrapper * VariableManager::getElement(string id) {
    pthread_mutex_lock(&mutex);
    VariableWrapper * managedElement = Manager<VariableWrapper,string>::getElement(id);
    pthread_mutex_unlock(&mutex);
    return managedElement;
}