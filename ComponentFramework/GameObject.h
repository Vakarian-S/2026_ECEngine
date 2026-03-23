#pragma once
#include <string>

class IHittable
{
public:
    virtual ~IHittable() = default;
    virtual void onHit(int damage);
};


class GameObject
{
protected:
    std::string name_;

public:
    GameObject();
    ~GameObject();

    std::string getName() const;
};

class DummyTarget : public GameObject, public IHittable
{
private:
    int hitsTaken_;

public:
    DummyTarget();
    ~DummyTarget();

    int hitsTaken();
    void onHit(int damage) override;
};

class Crate : public GameObject, public IHittable
{
private:
    int hp_;

public:
    Crate();
    ~Crate();

    int getHp();
    void onHit(int damage) override;
};

class Weapon : public GameObject
{
private:
    int damage_;

public:
    Weapon();
    ~Weapon();
    int getDamage();
    void swingAt(IHittable target);
};
