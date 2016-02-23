#!/bin/bash

## brew
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
echo 'brew installed'

## cmake
brew install cmake
echo 'cmake installed'

## command line tools
xcode-select --install
echo 'command line tools installed'


