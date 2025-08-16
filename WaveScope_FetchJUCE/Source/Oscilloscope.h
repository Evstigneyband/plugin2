#pragma once
#include <JuceHeader.h>

template<typename SampleType>
class AudioBufferQueue
{
public:
    explicit AudioBufferQueue (int capacitySamples)
        : fifo (capacitySamples),
          buffer (1, capacitySamples) {}

    void push (const SampleType* data, int numSamples)
    {
        int start1, size1, start2, size2;
        fifo.prepareToWrite (numSamples, start1, size1, start2, size2);
        if (size1 > 0) buffer.copyFrom (0, start1, data, size1);
        if (size2 > 0) buffer.copyFrom (0, start2, data + size1, size2);
        fifo.finishedWrite (size1 + size2);
    }

    int pop (SampleType* dest, int maxSamples)
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (maxSamples, start1, size1, start2, size2);
        int total = size1 + size2;
        if (size1 > 0) juce::FloatVectorOperations::copy (dest, buffer.getReadPointer(0, start1), size1);
        if (size2 > 0) juce::FloatVectorOperations::copy (dest + size1, buffer.getReadPointer(0, start2), size2);
        fifo.finishedRead (total);
        return total;
    }

    int getNumAvailable() const noexcept { return fifo.getNumReady(); }

private:
    juce::AbstractFifo fifo;
    juce::AudioBuffer<SampleType> buffer;
};

class OscilloscopeComponent : public juce::Component, private juce::Timer
{
public:
    OscilloscopeComponent (AudioBufferQueue<float>& leftQ, AudioBufferQueue<float>& rightQ)
        : leftQueue (leftQ), rightQueue (rightQ)
    {
        setOpaque (true);
        startTimerHz (60);
        reserve (displayLen);
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.fillAll (juce::Colours::black);
        g.setColour (juce::Colours::darkgrey.withAlpha (0.5f));
        for (int i = 1; i < 6; ++i)
        {
            float x = bounds.getX() + bounds.getWidth() * (float)i / 6.0f;
            g.drawVerticalLine ((int) x, bounds.getY(), bounds.getBottom());
        }
        g.drawHorizontalLine ((int) bounds.getCentreY(), bounds.getX(), bounds.getRight());

        float peak = juce::jmax (maxAbs (leftData), maxAbs (rightData), 0.1f);
        float scale = 0.9f / peak;

        auto top = bounds.removeFromTop (bounds.getHeight()/2.0f).reduced (4.0f);
        auto bottom = bounds.reduced (4.0f);
        drawWave (g, leftData, top, scale);
        drawWave (g, rightData, bottom, scale);
    }

private:
    AudioBufferQueue<float>& leftQueue;
    AudioBufferQueue<float>& rightQueue;
    std::vector<float> leftData, rightData, temp;
    static constexpr int displayLen = 2048;

    void reserve (int n) { leftData.assign(n,0.f); rightData.assign(n,0.f); temp.resize(n); }
    static float maxAbs (const std::vector<float>& v)
    { float m=0; for (auto s: v) m = juce::jmax (m, std::abs(s)); return m; }

    void pushSamplesFromQueue (AudioBufferQueue<float>& q, std::vector<float>& dest)
    {
        int toRead = juce::jmin (q.getNumAvailable(), 512);
        if (toRead <= 0) return;
        if ((int)temp.size() < toRead) temp.resize(toRead);
        int got = q.pop (temp.data(), toRead);
        if (got <= 0) return;

        if (got < (int)dest.size())
        {
            std::memmove (dest.data(), dest.data() + got, (dest.size()-got)*sizeof(float));
            std::memcpy (dest.data() + (dest.size()-got), temp.data(), got*sizeof(float));
        }
        else
        {
            std::memcpy (dest.data(), temp.data() + (got - (int)dest.size()), dest.size()*sizeof(float));
        }
    }

    void timerCallback() override
    {
        pushSamplesFromQueue (leftQueue, leftData);
        pushSamplesFromQueue (rightQueue, rightData);
        repaint();
    }

    void drawWave (juce::Graphics& g, const std::vector<float>& data, juce::Rectangle<float> area, float scale)
    {
        juce::Path p;
        int N = (int)data.size();
        if (N==0) return;
        auto midY = area.getCentreY();
        auto leftX = area.getX();
        auto rightX = area.getRight();
        float dx = (rightX-leftX)/(float)(N-1);
        p.startNewSubPath (leftX, midY - data[0]*scale*area.getHeight()/2.f);
        for (int i=1;i<N;++i)
            p.lineTo (leftX + dx*i, midY - data[i]*scale*area.getHeight()/2.f);
        g.setColour (juce::Colours::white);
        g.strokePath (p, juce::PathStrokeType (1.5f));
    }
};
