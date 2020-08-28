#/bin/bash

# Automatic build script
#
# To run locally, execute:
# sudo apt install doxygen graphviz texlive-latex-base texlive-latex-recommended texlive-pictures texlive-latex-extra

# Exit immediately if a command exits with a non-zero status.
set -e

echo "Starting auto-build script..."


function autobuild()
{
    # Set environment variables
    BOARDS_AVR="--board uno --board micro --board miniatmega328 --board nanoatmega328new --board pro16MHzatmega328 --board pro8MHzatmega328 --board megaatmega2560 --board leonardo"
    BOARDS_ESP="--board d1_mini --board nodemcuv2 --board lolin_d32"

    echo "Installing library dependencies"
    platformio lib --global install https://github.com/Erriez/ErriezTimestamp.git

    echo "Building examples..."
    platformio ci --lib="examples/Alarm" --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/Alarm/Alarm.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ESP} examples/Benchmark/Benchmark.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ESP} examples/GettingStarted/GettingStarted.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ESP} examples/PrintDateTime/PrintDateTime.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ESP} examples/RAM/RAM.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ESP} examples/SetTrickleCharger/SetTrickleCharger.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ESP} examples/SquareWave1Hz/SquareWave1Hz.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ESP} examples/Terminal/Terminal.ino
}

function generate_doxygen()
{
    echo "Generate Doxygen HTML..."

    DOXYGEN_PDF="ErriezDS1302.pdf"

    # Cleanup
    rm -rf html latex

    # Generate Doxygen HTML and Latex
    doxygen Doxyfile

    # Allow filenames starting with an underscore    
    echo "" > html/.nojekyll

    # Generate PDF when script is not running on Travis-CI
    if [[ -z ${TRAVIS_BUILD_DIR} ]]; then
        # Generate Doxygen PDF
        make -C latex

        # Copy PDF to root directory
        cp latex/refman.pdf ./${DOXYGEN_PDF}
    fi
}

autobuild
generate_doxygen

