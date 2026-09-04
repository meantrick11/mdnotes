# 使用ORM相关对象

在本节中，我们将介绍另一个重要的 ORM 概念，即 ORM 如何与引用其他对象的映射类进行交互。在声明映射类一节中，示例中的映射类用到了一种名为relationship()的结构。该结构用于定义两个不同映射类之间的关联，也可用于定义映射类与自身的关联，后者被称为**自引用关系**。

为了描述relationship()的基本思想，我们首先以简写形式回顾映射，省略mapped_column()映射及其他指令：
```python
from sqlalchemy.orm import Mapped
from sqlalchemy.orm import relationship


class User(Base):
    __tablename__ = "user_account"

    # ... mapped_column() mappings

    addresses: Mapped[List["Address"]] = relationship(back_populates="user")


class Address(Base):
    __tablename__ = "address"

    # ... mapped_column() mappings

    user: Mapped["User"] = relationship(back_populates="addresses")

```