from dataclasses import dataclass


@dataclass
class NavItem:
    name: str
    url_path: str


DEFAULT_NAV = [
    NavItem("data visualization", "datavis"),
    NavItem("table manager", "tablemanager"),
    NavItem("menu manager", "menumanager"),
    NavItem("current menu", "menu"),
    NavItem("live order backlog", "orderbacklog")
]
