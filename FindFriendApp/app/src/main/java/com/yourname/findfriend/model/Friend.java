package com.yourname.findfriend.model;

public class Friend {
    public String uid;
    public String name;
    public String avatar;
    public String tag;
    public double lat;
    public double lon;
    public boolean isCare;

    public Friend(String uid,String name,String tag,double lat,double lon) {
        this.uid=uid;
        this.name=name;
        this.tag=tag;
        this.lat=lat;
        this.lon=lon;
        this.avatar="default";
        this.isCare=false;
    }
}
