#pragma once
#include <iostream>
#include <cstdlib>

namespace stella {
namespace detail {

class StellaOutputProxy
{
public:
    StellaOutputProxy()
    {
        pOut_ = 0;
    }

    StellaOutputProxy(const StellaOutputProxy& other)
    {
        pOut_ = other.pOut_;
    }

    void Init(std::ostream& out)
    {
        pOut_ = &out;
    }

    template<typename T>
    StellaOutputProxy& operator<<(const T& obj)
    {
        if (pOut_)
            (*pOut_) << obj << std::flush;
        return *this;
    }

private:
    std::ostream* pOut_;

};

class StellaOutput
{
public:
    /**
     * Default constructor
     *
     * Initializes the object with the standard output
     * stream and reads the environmental variable
     * STELLA_DEBUG to find out the maximal level
     * ot sets it to if the variable is not defined.
     */
    StellaOutput()
    {
        pOut_ = &std::cout;
        level_ = 1;

        // Getting maxlevel from environment
        const char* pEnv = std::getenv("STELLA_DEBUG");
        if (!pEnv)
        {
            maxlevel_ = 0;
        }
        else
        {
            maxlevel_ = std::atoi(pEnv);
        }
    }

    /**
     * Copy constructor
     */
    StellaOutput(const StellaOutput& other)
    {
        pOut_ = other.pOut_;
        level_ = other.level_;
        maxlevel_ = other.maxlevel_;
    }

    /**
     * Output operator
     *
     * Writes the obj to the output stream
     * if the level is desired.
     *
     * @param obj An object that can be written to output streams
     *
     * @return The StellaOutput object itself is returned
     */
    template<typename T>
    StellaOutputProxy operator<<(const T& obj)
    {
        if (level_ > maxlevel_)
            return StellaOutputProxy();

        for (int l = 1; l < level_; ++l)
            (*pOut_) << "  ";

        if (level_ > 1) (*pOut_) << " -- ";

        StellaOutputProxy proxy;
        proxy.Init(*pOut_);
        proxy << obj;
        return proxy;
    }

    /**
     * Increases the current level by one
     */
    void Up() { level_ += 1; }

    /**
     * Decreases the current level by one
     */
    void Down()
    {
        if (level_ < 1)
            return;

        if (level_ <= maxlevel_)
            (*pOut_) << "\n";

        level_ -= 1;
    }

    /**
     * Creates an output object with the same properties 
     * but a different current level
     */
    StellaOutput operator()(int level)
    {
        StellaOutput ret;
        ret.Init(*pOut_, level, maxlevel_);
        return ret;
    }

    /**
     * Creates an output object with the same properties 
     * but with a different output stream
     */
    StellaOutput operator()(std::ostream& out)
    {
        StellaOutput ret;
        ret.Init(out, level_, maxlevel_);
        return ret;
    }

private:
    /**
     * Initializer
     *
     * Can not be called by the user: it is used internally
     * to deliver the API
     *
     * @param out An output stream that will be used
     * @param level The current level
     * @param maxlevel The maximal requested level of debug
     */
    void Init(std::ostream& out, int level, int maxlevel)
    {
        pOut_ = &out;
        level_ = level;
        maxlevel_ = maxlevel;
    }

    std::ostream* pOut_;
    int level_;
    int maxlevel_;
};

} // End of namespace detail
} // End of namespace stella

extern stella::detail::StellaOutput sout;

