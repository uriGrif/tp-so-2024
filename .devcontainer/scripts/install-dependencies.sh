sudo apt-get update

# install valgrind
sudo apt-get install valgrind -y

# install readline library
sudo apt-get install libreadline-dev -y

# install commons
git clone https://github.com/sisoputnfrba/so-commons-library.git commons
cd commons
make install
cd .. 
rm -rf commons

