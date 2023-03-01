// Tencent is pleased to support the open source community by making UnLua available.
// 
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the MIT License (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and limitations under the License.

#pragma once

#include "UObject/Interface.h"
#include "UnLuaInterface.generated.h"

/**
 * Interface for binding UCLASS and Lua module
 */
UINTERFACE()
class UNLUA_API UUnLuaInterface : public UInterface
{
    GENERATED_BODY()
};

class UNLUA_API IUnLuaInterface
{
    GENERATED_BODY()

public:
    /**
     * return a Lua file path which is relative to project's 'Content/Script', for example 'Weapon.BP_DefaultProjectile_C'
     */
    UFUNCTION(BlueprintNativeEvent)
    FString GetModuleName() const;
};
