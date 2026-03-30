package com.yourname.findfriend.adapter;

import android.content.Context;
import android.view.*;
import android.widget.*;
import androidx.recyclerview.widget.RecyclerView;
import com.yourname.findfriend.*;
import com.yourname.findfriend.model.Friend;
import java.util.List;

public class FriendAdapter extends RecyclerView.Adapter<FriendAdapter.VH> {
    Context ctx;
    List<Friend> list;
    OnClick cb;

    public FriendAdapter(Context c,List<Friend> l,OnClick click) {
        ctx=c; list=l; cb=click;
    }

    public static class VH extends RecyclerView.ViewHolder {
        TextView name,tag;
        public VH(View v) {
            super(v);
            name=v.findViewById(R.id.tv_username);
            tag=v.findViewById(R.id.tv_tag);
        }
    }

    @Override
    public VH onCreateViewHolder(ViewGroup p,int type) {
        return new VH(LayoutInflater.from(ctx).inflate(R.layout.item_friend,p,false));
    }

    @Override
    public void onBindViewHolder(VH h,int pos) {
        Friend f=list.get(pos);
        h.name.setText(f.name);
        h.tag.setText(f.tag);
        h.itemView.setOnClickListener(v->cb.onClick(f));
    }

    @Override public int getItemCount() { return list.size(); }
    public interface OnClick { void onClick(Friend f); }
}
