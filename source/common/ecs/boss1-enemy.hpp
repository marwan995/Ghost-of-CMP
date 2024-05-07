#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "../components/enemy-component.hpp"

#include <iostream>

namespace our
{
    class EnemyComponent;

    class Boss1
    {
        float rateOfFire = 2;
        int deltasCounter = 0;
        EnemyComponent* parent;

        public:
            Boss1(EnemyComponent* enemyComponent)
            {
                parent = enemyComponent;
            }

            void action()
            {
                if (parent->checkRateOfFire(rateOfFire, &deltasCounter))
                    std::cout<<"boss1\n";
            }

    };
}