#include "VideoListCtrl.hpp"
#include "Enums.hpp"
#include "VideoInfo.hpp"

VideoListCtrl::VideoListCtrl(wxWindow* parent) : wxListCtrl(parent, LIST_Video_list, wxDefaultPosition, wxSize(600,400), wxLC_REPORT | wxLC_SINGLE_SEL){
}

void VideoListCtrl::AddVideo(VideoInfo* video_data){
    VideoEntry entry(video_data);

    long int item_index = InsertItem(entry);
    SetItem(item_index, 0, wxString(video_data -> getName().c_str(),wxConvUTF8)); //want this for col. 0
    SetItem(item_index, 2, wxString(video_data -> getAuthor().c_str() , wxConvUTF8)); //col. 2

}
