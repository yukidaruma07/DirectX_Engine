#include "ObjectManager.h"
#include "CircleCollider.h"
#include "BoxCollider.h"

BaseObject* ObjectManager::AddObject(BaseObject* obj) {
    objList.push_back(obj);
    obj->Init();
    return obj;
}

void ObjectManager::RemoveObject(BaseObject* obj) {
    for (auto it = objList.begin(); it != objList.end(); ) {
        if ((*it) == obj) {
            delete* it;
            it = objList.erase(it);
        }
        else {
            it++;
        }
    }
}

void ObjectManager::ClearObject() {
    objList.clear();
}

void ObjectManager::UpdateManager() {
    for (int n = 0; n < objList.size(); n++) {
        auto obj = objList[n];
        if (obj == nullptr) continue;
        if (obj->IsDead()) {
            RemoveObject(obj);
        }
        else {
            obj->Update();
            UpdateCollider();
            obj->Draw();
            #ifdef _DEBUG
                if (!obj->IsShowImGUI()) return;
                obj->DrawObjectInfoImGUI();
            #endif
        }
    }
}

void ObjectManager::UpdateCollider() {
    for (int n = 0; n < objList.size(); n++) {
        for (int i = 1; i < objList.size(); i++) {
            auto obj1 = objList[n];
            auto obj2 = objList[i];
            
            bool isHit = false;
            for (Collider* cirlceCol1 : obj1->GetColiderList()) {
                cirlceCol1->Update();
                cirlceCol1->Draw();
                for (Collider* cirlceCol2 : obj2->GetColiderList()) {
                    cirlceCol2->Update();
                    cirlceCol2->Draw();

                    if (cirlceCol1->GetType() == ColliderType::CirlceCollider && cirlceCol2->GetType() == ColliderType::CirlceCollider) {
                        CircleCollider* aCol = dynamic_cast<CircleCollider*>(cirlceCol1);
                        CircleCollider* bCol = dynamic_cast<CircleCollider*>(cirlceCol2);
                        isHit = cirlceCol1->IsCirlce(aCol, bCol);
                    }
                    else if (cirlceCol1->GetType() == ColliderType::BoxCollider && cirlceCol2->GetType() == ColliderType::BoxCollider) {
                        BoxCollider* aCol = dynamic_cast<BoxCollider*>(cirlceCol1);
                        BoxCollider* bCol = dynamic_cast<BoxCollider*>(cirlceCol2);
                        isHit = cirlceCol1->IsBox(aCol, bCol);
                    }
                    else {
                        isHit = false;
                    }
                    
                }
            }

            if (isHit) {
                obj1->OnCollide(obj2);
                obj2->OnCollide(obj1);
            }
        }
    }
}
