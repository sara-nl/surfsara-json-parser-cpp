# JSON Parser for c++

A JSON parser and serializer for C11.

Features:
* parsing JSON from streams and buffers
* serialize JSON (compact or human readible)
* path expressions for searching and updating structures

## path expression

``` json
[
  {
    "name": "Meowsy",
    "species" : "cat",
    "foods": {
      "likes": ["tuna", "catnip"],
      "dislikes": ["ham", "zucchini"]
    }
  },
  {
    "name": "Barky",
    "species" : "dog",
    "foods": {
      "likes": ["bones", "carrots"],
      "dislikes": ["tuna"]
    }
  },
  {
    "name": "Purrpaws",
    "species" : "cat",
    "foods": {
      "likes": ["mice"],
      "dislikes": ["cookies"]
    }
  }
]
```

```
0/species -> "cat"
1/foods/dislikes/0 -> "tuna"
```


```
update('0/foods/dislikes/#', String("veggies"))
remove('0/foods/likes/0')
update('*/foods/dislikes/#', String("brocoli"))
update('*/*/likes/#', String("dog-food"), []((Object obj1, Index), (Object obj2, Index) { return true if(obj1.species == "dog" && key2=="foods") })
```


