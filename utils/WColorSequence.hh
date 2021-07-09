/* WPngImage
   ---------
   This source code is released under the MIT license.
   For full documentation, consult the WColorSequence.html file.
*/

#ifndef WCOLORSEQUENCE_INCLUDE_GUARD
#define WCOLORSEQUENCE_INCLUDE_GUARD
#include "../WPngImage.hh"
#include <vector>
#include <utility>
#include <initializer_list>
#include <cmath>

template<typename> class WColorSequence;

using WColorSequence8 = WColorSequence<WPngImage::Pixel8>;
using WColorSequence16 = WColorSequence<WPngImage::Pixel16>;
using WColorSequenceF = WColorSequence<WPngImage::PixelF>;

template<typename Pixel_t>
class WColorSequence
{
 public:
    //------------------------------------------------------------------------
    // Types
    //------------------------------------------------------------------------
    typedef double Float;

    struct Entry
    {
        Pixel_t color;
        Float length;

        Entry(): length(1.0) {}
        Entry(Pixel_t c): color(c), length(1.0) {}
        Entry(Pixel_t c, Float l): color(c), length(l) {}
    };

    enum class MappingType { clamp, cyclic, repeating, mirroring };

    struct Settings
    {
        MappingType mappingType = MappingType::repeating;
        bool normalizedMapping = true;
        bool alphaAwareInterpolation = true;

        Settings() {}
        Settings(MappingType t, bool n, bool a):
            mappingType(t), normalizedMapping(n), alphaAwareInterpolation(a) {}
    };


    //------------------------------------------------------------------------
    // Initialization
    //------------------------------------------------------------------------
    WColorSequence() {}
    WColorSequence(Settings);
    WColorSequence(const Entry*, std::size_t amount, Settings, bool makeCopy);
    void initialize(const Entry*, std::size_t amount, bool makeCopy);

    WColorSequence(const std::vector<Entry>&, Settings, bool makeCopy);
    WColorSequence(std::vector<Entry>&&, Settings);
    void initialize(const std::vector<Entry>&, bool makeCopy);
    void initialize(std::vector<Entry>&&);

    WColorSequence(std::initializer_list<Entry>, Settings, bool makeCopy);
    void initialize(std::initializer_list<Entry>, bool makeCopy);

    template<std::size_t kArraySize>
    WColorSequence(const Entry(&)[kArraySize], Settings, bool makeCopy);

    template<std::size_t kArraySize>
    void initialize(const Entry(&)[kArraySize], bool makeCopy);

    template<typename InputIterator>
    WColorSequence(InputIterator b, InputIterator e, Settings);

    template<typename InputIterator>
    void initialize(InputIterator b, InputIterator e);

    void setSettings(Settings);

    WColorSequence(const WColorSequence&);
    WColorSequence(WColorSequence&&) noexcept;
    WColorSequence& operator=(const WColorSequence&);
    WColorSequence& operator=(WColorSequence&&) noexcept;


    //------------------------------------------------------------------------
    // Data getters
    //------------------------------------------------------------------------
    bool isNonOwning() const;
    const Entry* entriesPtr() const;
    std::size_t entriesAmount() const;
    Settings currentSettings() const;

    Pixel_t getInterpolatedPixel(Float position) const;


 private:
    //------------------------------------------------------------------------
    // Private data
    //------------------------------------------------------------------------
    std::vector<Entry> mEntries;
    const Entry* mEntriesPtr = nullptr;
    std::size_t mEntriesAmount = 0;
    Settings mSettings;
    Float mSumOfLengths = 0;

    void initializeData();
    static typename Pixel_t::Component_t getCTFactor(Float);
};




//============================================================================
// WColorSequence implementations
//============================================================================
template<typename Pixel_t>
WColorSequence<Pixel_t>::WColorSequence(Settings settings):
    mSettings(settings)
{}

template<typename Pixel_t>
WColorSequence<Pixel_t>::WColorSequence
(const Entry* entries, std::size_t amount, Settings settings, bool makeCopy):
    mSettings(settings)
{
    initialize(entries, amount, makeCopy);
}

template<typename Pixel_t>
void WColorSequence<Pixel_t>::initialize
(const Entry* entries, std::size_t amount, bool makeCopy)
{
    if(makeCopy)
    {
        mEntries.assign(entries, entries + amount);
        mEntriesPtr = &mEntries[0];
    }
    else
    {
        mEntries.clear();
        mEntriesPtr = entries;
    }

    mEntriesAmount = amount;
    initializeData();
}

template<typename Pixel_t>
WColorSequence<Pixel_t>::WColorSequence
(const std::vector<Entry>& entries, Settings settings, bool makeCopy):
    mSettings(settings)
{
    initialize(entries, makeCopy);
}

template<typename Pixel_t>
void WColorSequence<Pixel_t>::initialize(const std::vector<Entry>& entries, bool makeCopy)
{
    if(makeCopy)
    {
        mEntries = entries;
        mEntriesPtr = &mEntries[0];
    }
    else
    {
        mEntries.clear();
        mEntriesPtr = &entries[0];
    }

    mEntriesAmount = entries.size();
    initializeData();
}

template<typename Pixel_t>
WColorSequence<Pixel_t>::WColorSequence(std::vector<Entry>&& entries, Settings settings):
    mEntries(std::move(entries)),
    mEntriesPtr(&mEntries[0]),
    mEntriesAmount(mEntries.size()),
    mSettings(settings)
{
    initializeData();
}

template<typename Pixel_t>
void WColorSequence<Pixel_t>::initialize(std::vector<Entry>&& entries)
{
    mEntries = std::move(entries);
    mEntries = &mEntries[0];
    mEntriesAmount = mEntries.size();
    initializeData();
}

template<typename Pixel_t>
WColorSequence<Pixel_t>::WColorSequence
(std::initializer_list<Entry> entries, Settings settings, bool makeCopy):
    mSettings(settings)
{
    initialize(entries, makeCopy);
}

template<typename Pixel_t>
void WColorSequence<Pixel_t>::initialize
(std::initializer_list<Entry> entries, bool makeCopy)
{
    initialize(entries.begin(), entries.size(), makeCopy);
}

template<typename Pixel_t>
template<std::size_t kArraySize>
WColorSequence<Pixel_t>::WColorSequence
(const Entry(&entries)[kArraySize], Settings settings, bool makeCopy):
    mSettings(settings)
{
    initialize(static_cast<const Entry*>(entries), kArraySize, makeCopy);
}

template<typename Pixel_t>
template<std::size_t kArraySize>
void WColorSequence<Pixel_t>::initialize(const Entry(&entries)[kArraySize], bool makeCopy)
{
    initialize(static_cast<const Entry*>(entries), kArraySize, makeCopy);
}

template<typename Pixel_t>
template<typename InputIterator>
WColorSequence<Pixel_t>::WColorSequence(InputIterator b, InputIterator e, Settings settings):
    mEntries(b, e),
    mEntriesPtr(&mEntries[0]),
    mEntriesAmount(mEntries.size()),
    mSettings(settings)
{
    initializeData();
}

template<typename Pixel_t>
template<typename InputIterator>
void WColorSequence<Pixel_t>::initialize(InputIterator b, InputIterator e)
{
    mEntries.assign(b, e);
    mEntriesPtr = &mEntries[0];
    mEntriesAmount = mEntries.size();
    initializeData();
}

template<typename Pixel_t>
void WColorSequence<Pixel_t>::setSettings(Settings settings)
{
    mSettings = settings;
    initializeData();
}

template<typename Pixel_t>
WColorSequence<Pixel_t>::WColorSequence(const WColorSequence& rhs):
    mEntries(rhs.mEntries),
    mEntriesPtr(mEntries.empty() ? rhs.mEntriesPtr : &mEntries[0]),
    mEntriesAmount(rhs.mEntriesAmount),
    mSettings(rhs.mSettings),
    mSumOfLengths(rhs.mSumOfLengths)
{}

template<typename Pixel_t>
WColorSequence<Pixel_t>& WColorSequence<Pixel_t>::operator=(const WColorSequence& rhs)
{
    mEntries = rhs.mEntries;
    mEntriesPtr = (mEntries.empty() ? rhs.mEntriesPtr : &mEntries[0]);
    mEntriesAmount = rhs.mEntriesAmount;
    mSettings = rhs.mSettings;
    mSumOfLengths = rhs.mSumOfLengths;
    return *this;
}

template<typename Pixel_t>
WColorSequence<Pixel_t>::WColorSequence(WColorSequence&& rhs) noexcept:
    mEntries(std::move(rhs.mEntries)),
    mEntriesPtr(mEntries.empty() ? rhs.mEntriesPtr : &mEntries[0]),
    mEntriesAmount(rhs.mEntriesAmount),
    mSettings(rhs.mSettings),
    mSumOfLengths(rhs.mSumOfLengths)
{}

template<typename Pixel_t>
WColorSequence<Pixel_t>& WColorSequence<Pixel_t>::operator=(WColorSequence&& rhs) noexcept
{
    mEntries = std::move(rhs.mEntries);
    mEntriesPtr = (mEntries.empty() ? rhs.mEntriesPtr : &mEntries[0]);
    mEntriesAmount = rhs.mEntriesAmount;
    mSettings = rhs.mSettings;
    mSumOfLengths = rhs.mSumOfLengths;
    return *this;
}

template<typename Pixel_t>
void WColorSequence<Pixel_t>::initializeData()
{
    mSumOfLengths = 0;
    if(mEntriesAmount > 0)
    {
        switch(mSettings.mappingType)
        {
          case MappingType::clamp:
          case MappingType::cyclic:
          case MappingType::mirroring:
              for(std::size_t i = 0; i < mEntriesAmount - 1; ++i)
                  mSumOfLengths += mEntriesPtr[i].length;
              break;

          case MappingType::repeating:
              for(std::size_t i = 0; i < mEntriesAmount; ++i)
                  mSumOfLengths += mEntriesPtr[i].length;
              break;
        }
    }

    if(mSumOfLengths < 1.0e-6) mSumOfLengths = 1.0e-6;
}

template<typename Pixel_t>
bool WColorSequence<Pixel_t>::isNonOwning() const
{
    return mEntriesAmount > 0 && mEntries.empty();
}

template<typename Pixel_t>
const typename WColorSequence<Pixel_t>::Entry* WColorSequence<Pixel_t>::entriesPtr() const
{
    return mEntriesPtr;
}

template<typename Pixel_t>
std::size_t WColorSequence<Pixel_t>::entriesAmount() const
{
    return mEntriesAmount;
}

template<typename Pixel_t>
typename WColorSequence<Pixel_t>::Settings WColorSequence<Pixel_t>::currentSettings() const
{
    return mSettings;
}

template<>
inline WPngImage::Pixel8::Component_t WColorSequence<WPngImage::Pixel8>::getCTFactor(Float f)
{
    return static_cast<WPngImage::Pixel8::Component_t>
        (f * static_cast<Float>(WPngImage::Pixel8::kComponentMaxValue));
}

template<>
inline WPngImage::Pixel16::Component_t WColorSequence<WPngImage::Pixel16>::getCTFactor(Float f)
{
    return static_cast<WPngImage::Pixel16::Component_t>
        (f * static_cast<Float>(WPngImage::Pixel16::kComponentMaxValue));
}

template<>
inline WPngImage::PixelF::Component_t WColorSequence<WPngImage::PixelF>::getCTFactor(Float f)
{
    return f;
}

template<typename Pixel_t>
Pixel_t WColorSequence<Pixel_t>::getInterpolatedPixel(Float position) const
{
    if(mEntriesAmount == 0) return {};
    if(mEntriesAmount == 1) return mEntriesPtr[0].color;

    if(mSettings.normalizedMapping)
        position *= mSumOfLengths;

    const std::size_t lastEntryIndex = mEntriesAmount - 1;

    if(mSettings.mappingType == MappingType::clamp)
    {
        if(position <= 0.0) return mEntriesPtr[0].color;
        if(position >= mSumOfLengths) return mEntriesPtr[lastEntryIndex].color;
    }

    const std::size_t maxPositionIndex =
        (mSettings.mappingType == MappingType::cyclic ?
         lastEntryIndex - 1 : lastEntryIndex);

    if(mSettings.mappingType == MappingType::mirroring)
    {
        const Float doubleSumOfLengths = mSumOfLengths * 2.0;
        position = std::fmod(position, doubleSumOfLengths);
        if(position < 0.0) position += doubleSumOfLengths;
        if(position >= mSumOfLengths)
            position = doubleSumOfLengths - position;
    }
    else
    {
        position = std::fmod(position, mSumOfLengths);
        if(position < 0.0) position += mSumOfLengths;
    }

    std::size_t positionIndex = 0;
    Float prevSum = 0, nextSum = mEntriesPtr[0].length;
    while(positionIndex < maxPositionIndex && nextSum < position)
    {
        prevSum = nextSum;
        nextSum += mEntriesPtr[++positionIndex].length;
    }

    const Pixel_t prevColor = mEntriesPtr[positionIndex].color;
    const Float sumRange = nextSum - prevSum;
    if(sumRange < 1.0e-6) return prevColor;

    const Pixel_t nextColor = (positionIndex == lastEntryIndex ?
                               mEntriesPtr[0].color :
                               mEntriesPtr[positionIndex + 1].color);

    const Float factor = (position - prevSum) / sumRange;
    const typename Pixel_t::Component_t ctFactor = getCTFactor(factor);

    return (mSettings.alphaAwareInterpolation ?
            prevColor.interpolatedPixel(nextColor, ctFactor) :
            prevColor.rawInterpolatedPixel(nextColor, ctFactor));
}
#endif
