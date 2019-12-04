#ifndef DELTATIME_H
#define DELTATIME_H
namespace cjk {
class DeltaTime {
public:
    DeltaTime(float time = 0.0f) : mTime(time)
    {
    }
    operator float() const { return mTime; }
    float getSeconds() const { return mTime; }
    float getMilliSeconds() const { return mTime * 1000.f; }

private:
    float mTime;
};
} // namespace cjk

#endif // DELTATIME_H
