
quiry = {
    "type":"object",
    "properties":{
        "person":{
            "type":"string",
            "description":"查询人的邮箱"
        },
        "suject":{
            "type":"string",
            "description":"查询的物品"
        },
        "body":{
            "type":"string",
            "description":"查询的内容"
        },
        "priority":{
            "type":"string",
            "enum":["low","normal","high"],
            "default":"normal"
        },


      

    }
   
}