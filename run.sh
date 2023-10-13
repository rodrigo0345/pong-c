
#!/bin/bash

cd build

cmake ..
make

./app $@


