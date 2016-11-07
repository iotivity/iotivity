package org.iotivity.cloud.accountserver.db;

public class UserTable {
    private String uuid     = null;
    private String userid   = null;
    private String provider = null;
    private String email    = null;
    private String phone    = null;

    public String getUuid() {
        return uuid;
    }

    public void setUuid(Object uuid) {
        if (uuid != null)
            this.uuid = uuid.toString();
    }

    public String getUserid() {
        return userid;
    }

    public void setUserid(Object userid) {
        if (userid != null)
            this.userid = userid.toString();
    }

    public String getProvider() {
        return provider;
    }

    public void setProvider(Object provider) {
        if (provider != null)
            this.provider = provider.toString();
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(Object email) {
        if (email != null)
            this.email = email.toString();
    }

    public String getPhone() {
        return phone;
    }

    public void setPhone(Object phone) {
        if (phone != null)
            this.phone = phone.toString();
    }

}
