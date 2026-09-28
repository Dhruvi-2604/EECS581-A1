#include <iostream>
#include <string>
#include <cctype>

/*
 * Extracts one valid IPv4 address and an optional port
 * from a line of text.
 *
 * Returns true if exactly one valid IPv4 address is found.
 *
 * On success:
 *   outAddress = decimal 32-bit IPv4 value
 *   outPort = port number, or -1 if no port exists
 *
 * On failure:
 *   outAddress = 0
 *   outPort = -1
 */
bool extractIPv4(const std::string& str,
                 unsigned long& outAddress,
                 int& outPort)
{
    outAddress = 0;
    outPort = -1;

    bool foundValid = false;
    unsigned long tempAddress = 0;
    int tempPort = -1;

    size_t i = 0;
    size_t n = str.length();

    while (i < n)
    {
        /*
         * Characters other than digits, periods, and colons
         * are garbage and are skipped.
         */
        if (!std::isdigit(static_cast<unsigned char>(str[i])) &&
            str[i] != '.' &&
            str[i] != ':')
        {
            ++i;
            continue;
        }

        // Find the end of the candidate token.
        size_t start = i;

        while (i < n &&
               (std::isdigit(static_cast<unsigned char>(str[i])) ||
                str[i] == '.' ||
                str[i] == ':'))
        {
            ++i;
        }

        size_t end = i;
        size_t pos = start;

        bool validCandidate = true;
        unsigned long octets[4] = {0, 0, 0, 0};

        /*
         * Parse the four IPv4 octets.
         */
        for (int octetIndex = 0;
             octetIndex < 4;
             ++octetIndex)
        {
            // Each octet must begin with a digit.
            if (pos >= end ||
                !std::isdigit(
                    static_cast<unsigned char>(str[pos])))
            {
                validCandidate = false;
                break;
            }

            size_t digitStart = pos;
            unsigned long value = 0;

            // Accumulate the digits manually.
            while (pos < end &&
                   std::isdigit(
                       static_cast<unsigned char>(str[pos])))
            {
                value = value * 10 + (str[pos] - '0');
                ++pos;
            }

            size_t digitLength = pos - digitStart;

            // Octet must contain 1 to 3 digits.
            if (digitLength < 1 || digitLength > 3)
            {
                validCandidate = false;
                break;
            }

            // Octet must be between 0 and 255.
            if (value > 255)
            {
                validCandidate = false;
                break;
            }

            // Leading zero is not allowed except for "0".
            if (digitLength > 1 &&
                str[digitStart] == '0')
            {
                validCandidate = false;
                break;
            }

            octets[octetIndex] = value;

            /*
             * The first three octets must be followed
             * immediately by a period.
             */
            if (octetIndex < 3)
            {
                if (pos < end && str[pos] == '.')
                {
                    ++pos;
                }
                else
                {
                    validCandidate = false;
                    break;
                }
            }
        }

        if (!validCandidate)
        {
            continue;
        }

        /*
         * Check for an optional port.
         */
        int parsedPort = -1;

        if (pos < end)
        {
            if (str[pos] == ':')
            {
                ++pos;

                // A colon must be followed by at least one digit.
                if (pos >= end ||
                    !std::isdigit(
                        static_cast<unsigned char>(str[pos])))
                {
                    validCandidate = false;
                }
                else
                {
                    size_t portStart = pos;
                    unsigned long portValue = 0;

                    // Accumulate port digits manually.
                    while (pos < end &&
                           std::isdigit(
                               static_cast<unsigned char>(str[pos])))
                    {
                        portValue = portValue * 10 +
                                    (str[pos] - '0');
                        ++pos;
                    }

                    size_t portLength = pos - portStart;

                    // Port must contain 1 to 5 digits.
                    if (portLength < 1 || portLength > 5)
                    {
                        validCandidate = false;
                    }
                    // Port must be between 0 and 65535.
                    else if (portValue > 65535)
                    {
                        validCandidate = false;
                    }
                    // Leading zero is not allowed except for "0".
                    else if (portLength > 1 &&
                             str[portStart] == '0')
                    {
                        validCandidate = false;
                    }
                    else
                    {
                        parsedPort =
                            static_cast<int>(portValue);
                    }
                }
            }
            else
            {
                /*
                 * Anything remaining in the candidate token
                 * makes the candidate invalid.
                 */
                validCandidate = false;
            }
        }

        /*
         * The complete candidate must have been consumed.
         */
        if (validCandidate && pos == end)
        {
            /*
             * The assignment requires exactly one valid
             * IPv4 address per input line.
             */
            if (foundValid)
            {
                outAddress = 0;
                outPort = -1;
                return false;
            }

            /*
             * Convert A.B.C.D to its 32-bit decimal value
             * without using a conversion function.
             */
            tempAddress =
                octets[0] * 16777216UL +
                octets[1] * 65536UL +
                octets[2] * 256UL +
                octets[3];

            tempPort = parsedPort;
            foundValid = true;
        }
    }

    if (foundValid)
    {
        outAddress = tempAddress;
        outPort = tempPort;
        return true;
    }

    outAddress = 0;
    outPort = -1;

    return false;
}


int main()
{
    std::string line;

    while (true)
    {
        std::cout << "Enter a string (or 'END' to quit): ";

        if (!std::getline(std::cin, line))
        {
            break;
        }

        if (line == "END")
        {
            std::cout << "Program terminated." << std::endl;
            break;
        }

        unsigned long address = 0;
        int port = -1;

        if (extractIPv4(line, address, port))
        {
            /*
             * Convert the decimal address back into
             * four octets for display.
             */
            unsigned long a =
                (address / 16777216UL) % 256;

            unsigned long b =
                (address / 65536UL) % 256;

            unsigned long c =
                (address / 256UL) % 256;

            unsigned long d =
                address % 256;

            std::cout
                << "Extracted IPv4 address: "
                << a << "."
                << b << "."
                << c << "."
                << d
                << " (decimal value: "
                << address
                << ", port: ";

            if (port == -1)
            {
                std::cout << "none";
            }
            else
            {
                std::cout << port;
            }

            std::cout << ")" << std::endl;
        }
        else
        {
            std::cout
                << "Invalid input: no valid IPv4 address found"
                << std::endl;
        }
    }

    return 0;
}
