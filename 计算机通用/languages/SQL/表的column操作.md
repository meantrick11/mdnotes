# 表的column操作

## Adding column

```sql
ALTER TABLE mytable
ADD column DataType OptionalTableConstraint 
    DEFAULT default_value;
```


## Removing column
```sql
ALTER TABLE mytable
DROP column_to_be_deleted;
```

## Renaming column

```sql
ALTER TABLE mytable
RENAME TO new_table_name;
```


