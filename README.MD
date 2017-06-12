# GET_DATA_FROM_NODE
A UDTF that allows fetching data from a single Vertica node


## Compile

```
mkdir build && cd build
cmake ..
make
```

## Install

```
CREATE OR REPLACE LIBRARY libgetdatafromnode 
  AS '/home/dbadmin/libgetdatafromnode.so';
CREATE OR REPLACE TRANSFORM FUNCTION GET_DATA_FROM_NODE 
  AS LANGUAGE 'C++' 
  NAME 'GetDataFromNodeFactory' 
  LIBRARY libgetdatafromnode 
  NOT FENCED;
```

## Use

```
SELECT GET_DATA_FROM_NODE(* USING PARAMETERS node='v_foo_node0001') OVER (PARTITION AUTO)
FROM public.bar
```


