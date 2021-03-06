/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "stdafx.h"
#include "GUIWindowMusicNav.h"
#include "Util.h"
#include "utils/GUIInfoManager.h"
#include "PlayListM3U.h"
#include "PlayListPlayer.h"
#include "GUIPassword.h"
#ifdef PRE_SKIN_VERSION_2_1_COMPATIBILITY
#include "GUILabelControl.h"
#include "GUIFontManager.h"
#endif
#include "GUIDialogFileBrowser.h"
#include "GUIDialogContentSettings.h"
#include "Picture.h"
#include "PartyModeManager.h"
#include "PlayListFactory.h"
#include "VideoDatabase.h"
#include "GUIWindowVideoNav.h"
#include "MusicInfoTag.h"
#include "GUIWindowManager.h"
#include "GUIDialogOK.h"
#include "GUIDialogKeyboard.h"
#include "FileSystem/File.h"
#include "FileItem.h"
#include "Application.h"

using namespace std;
using namespace DIRECTORY;
using namespace PLAYLIST;

#define CONTROL_BTNVIEWASICONS     2
#define CONTROL_BTNSORTBY          3
#define CONTROL_BTNSORTASC         4
#define CONTROL_BTNTYPE            5
#define CONTROL_LIST              50
#define CONTROL_THUMBS            51
#define CONTROL_BIGLIST           52
#define CONTROL_LABELFILES        12

#define CONTROL_BTNSEARCH          8
#define CONTROL_FILTER            15
#define CONTROL_BTNPARTYMODE      16
#define CONTROL_BTNMANUALINFO     17
#define CONTROL_BTN_FILTER        19
#define CONTROL_LABELEMPTY        18

CGUIWindowMusicNav::CGUIWindowMusicNav(void)
    : CGUIWindowMusicBase(WINDOW_MUSIC_NAV, "MyMusicNav.xml")
{
  m_vecItems->m_strPath = "?";
  m_bDisplayEmptyDatabaseMessage = false;
  m_thumbLoader.SetObserver(this);
  m_unfilteredItems = new CFileItemList;
}

CGUIWindowMusicNav::~CGUIWindowMusicNav(void)
{
  delete m_unfilteredItems;
}

bool CGUIWindowMusicNav::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
  case GUI_MSG_WINDOW_RESET:
    m_vecItems->m_strPath = "?";
    break;
  case GUI_MSG_WINDOW_DEINIT:
    if (m_thumbLoader.IsLoading())
      m_thumbLoader.StopThread();
    break;
  case GUI_MSG_WINDOW_INIT:
    {
/* We don't want to show Autosourced items (ie removable pendrives, memorycards) in Library mode */
      m_rootDir.AllowNonLocalSources(false);
      // check for valid quickpath parameter
      CStdString strDestination = message.GetStringParam();
      if (!strDestination.IsEmpty())
      {
        message.SetStringParam("");
        CLog::Log(LOGINFO, "Attempting to quickpath to: %s", strDestination.c_str());
      }

      // is this the first time the window is opened?
      if (m_vecItems->m_strPath == "?" && strDestination.IsEmpty())
      {
        strDestination = g_settings.m_defaultMusicLibSource;
        m_vecItems->m_strPath = strDestination;
        CLog::Log(LOGINFO, "Attempting to default to: %s", strDestination.c_str());
      }

      if (!strDestination.IsEmpty())
      {
        if (strDestination.Equals("$ROOT") || strDestination.Equals("Root"))
        {
          m_vecItems->m_strPath = "";
        }
        else if (strDestination.Equals("Genres"))
        {
          m_vecItems->m_strPath = "musicdb://1/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Artists"))
        {
          m_vecItems->m_strPath = "musicdb://2/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Albums"))
        {
          m_vecItems->m_strPath = "musicdb://3/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Songs"))
        {
          m_vecItems->m_strPath = "musicdb://4/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Top100"))
        {
          m_vecItems->m_strPath = "musicdb://5/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Top100Songs"))
        {
          m_vecItems->m_strPath = "musicdb://5/2/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Top100Albums"))
        {
          m_vecItems->m_strPath = "musicdb://5/1/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("RecentlyAddedAlbums"))
        {
          m_vecItems->m_strPath = "musicdb://6/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("RecentlyPlayedAlbums"))
        {
          m_vecItems->m_strPath = "musicdb://7/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Compilations"))
        {
          m_vecItems->m_strPath = "musicdb://8/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Playlists"))
        {
          m_vecItems->m_strPath = "special://musicplaylists/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Years"))
        {
          m_vecItems->m_strPath = "musicdb://9/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else if (strDestination.Equals("Plugins"))
        {
          m_vecItems->m_strPath = "plugin://music/";
          SetHistoryForPath(m_vecItems->m_strPath);
        }
        else
        {
          CLog::Log(LOGWARNING, "Warning, destination parameter (%s) may not be valid", strDestination.c_str());
          m_vecItems->m_strPath = strDestination;
          SetHistoryForPath(m_vecItems->m_strPath);
          break;
        }
      }

      DisplayEmptyDatabaseMessage(false); // reset message state

      if (!CGUIWindowMusicBase::OnMessage(message))
        return false;

      //  base class has opened the database, do our check
      DisplayEmptyDatabaseMessage(m_musicdatabase.GetSongsCount() <= 0);

      if (m_bDisplayEmptyDatabaseMessage)
      {
        // no library - make sure we focus on a known control, and default to the root.
        SET_CONTROL_FOCUS(CONTROL_BTNTYPE, 0);
        m_vecItems->m_strPath = "";
        SetHistoryForPath("");
        Update("");
      }

      return true;
    }
    break;

  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      if (iControl == CONTROL_BTNPARTYMODE)
      {
        if (g_partyModeManager.IsEnabled())
          g_partyModeManager.Disable();
        else
        {
          if (!g_partyModeManager.Enable())
          {
            SET_CONTROL_SELECTED(GetID(),CONTROL_BTNPARTYMODE,false);
            return false;
          }

          // Playlist directory is the root of the playlist window
          if (m_guiState.get()) m_guiState->SetPlaylistDirectory("playlistmusic://");

          return true;
        }
        UpdateButtons();
      }
      else if (iControl == CONTROL_BTNMANUALINFO)
      {
        OnManualAlbumInfo();
        return true;
      }
      else if (iControl == CONTROL_BTN_FILTER)
      {
        if (m_filter.IsEmpty())
          CGUIDialogKeyboard::ShowAndGetFilter(m_filter, false);
        else
        {
          m_filter.Empty();
          OnFilterItems();
        }
        return true;
      }
      else if (iControl == CONTROL_BTNSEARCH)
      {
        OnSearch();
        return true;
      }
    }
    break;
  case GUI_MSG_PLAYBACK_STOPPED:
  case GUI_MSG_PLAYBACK_ENDED:
  case GUI_MSG_PLAYLISTPLAYER_STOPPED:
  case GUI_MSG_PLAYBACK_STARTED:
    {
      SET_CONTROL_SELECTED(GetID(),CONTROL_BTNPARTYMODE, g_partyModeManager.IsEnabled());
    }
    break;
  case GUI_MSG_NOTIFY_ALL:
    {
      if (message.GetParam1() == GUI_MSG_FILTER_ITEMS && IsActive())
      {
        if (message.GetParam2() == 1) // append
          m_filter += message.GetStringParam();
        else if (message.GetParam2() == 2)
        { // delete
          if (m_filter.size())
            m_filter = m_filter.Left(m_filter.size() - 1);
        }
        else
          m_filter = message.GetStringParam();
        OnFilterItems();
        return true;
      }
      if (message.GetParam1() == GUI_MSG_SEARCH_UPDATE && IsActive())
      {
        m_search = message.GetStringParam();
        CUtil::URLEncode(m_search);
        if (!m_search.IsEmpty())
        {
          CStdString path = "musicsearch://" + m_search + "/";
          m_history.ClearPathHistory();
          Update(path);
        }
      }
    }
  }
  return CGUIWindowMusicBase::OnMessage(message);
}

bool CGUIWindowMusicNav::OnAction(const CAction& action)
{
  return CGUIWindowMusicBase::OnAction(action);
}

CStdString CGUIWindowMusicNav::GetQuickpathName(const CStdString& strPath) const
{
  if (strPath.Equals("musicdb://1/"))
    return "Genres";
  else if (strPath.Equals("musicdb://2/"))
    return "Artists";
  else if (strPath.Equals("musicdb://3/"))
    return "Albums";
  else if (strPath.Equals("musicdb://4/"))
    return "Songs";
  else if (strPath.Equals("musicdb://5/"))
    return "Top100";
  else if (strPath.Equals("musicdb://5/2/"))
    return "Top100Songs";
  else if (strPath.Equals("musicdb://5/1/"))
    return "Top100Albums";
  else if (strPath.Equals("musicdb://6/"))
    return "RecentlyAddedAlbums";
  else if (strPath.Equals("musicdb://7/"))
    return "RecentlyPlayedAlbums";
  else if (strPath.Equals("musicdb://8/"))
    return "Compilations";
  else if (strPath.Equals("musicdb://9/"))
    return "Years";
  else if (strPath.Equals("special://musicplaylists/"))
    return "Playlists";
  else
  {
    CLog::Log(LOGERROR, "  CGUIWindowMusicNav::GetQuickpathName: Unknown parameter (%s)", strPath.c_str());
    return strPath;
  }
}

bool CGUIWindowMusicNav::OnClick(int iItem)
{
  if (iItem < 0 || iItem >= m_vecItems->Size()) return false;

  CFileItemPtr item = m_vecItems->Get(iItem);
  if (item->m_strPath.Left(14) == "musicsearch://")
  {
    OnSearch();
    return true;
  }
  return CGUIWindowMusicBase::OnClick(iItem);
}

void CGUIWindowMusicNav::OnSearch()
{
  CStdString search(m_search);
  CUtil::UrlDecode(search);
  CGUIDialogKeyboard::ShowAndGetFilter(search, true);
}

bool CGUIWindowMusicNav::GetDirectory(const CStdString &strDirectory, CFileItemList &items)
{
  if (m_bDisplayEmptyDatabaseMessage)
    return true;

  if (m_thumbLoader.IsLoading())
    m_thumbLoader.StopThread();

  bool bResult = CGUIWindowMusicBase::GetDirectory(strDirectory, items);
  if (bResult)
  {
    if (items.IsPlayList())
      OnRetrieveMusicInfo(items);
    if (!items.IsMusicDb())
    {
      items.SetCachedMusicThumbs();
      m_thumbLoader.Load(*m_vecItems);
    }
  }

  if (strDirectory.Equals("special://musicplaylists"))
    items.SetContent("playlists");
  else if (strDirectory.Equals("plugin://music/"))
    items.SetContent("plugins");

  // clear the filter
  m_filter.Empty();
  return bResult;
}

void CGUIWindowMusicNav::UpdateButtons()
{
  CGUIWindowMusicBase::UpdateButtons();

  // Update object count
  int iItems = m_vecItems->Size();
  if (iItems)
  {
    // check for parent dir and "all" items
    // should always be the first two items
    for (int i = 0; i <= (iItems>=2 ? 1 : 0); i++)
    {
      CFileItemPtr pItem = m_vecItems->Get(i);
      if (pItem->IsParentFolder()) iItems--;
      if (pItem->m_strPath.Left(4).Equals("/-1/")) iItems--;
    }
    // or the last item
    if (m_vecItems->Size() > 2 &&
      m_vecItems->Get(m_vecItems->Size()-1)->m_strPath.Left(4).Equals("/-1/"))
      iItems--;
  }
  CStdString items;
  items.Format("%i %s", iItems, g_localizeStrings.Get(127).c_str());
  SET_CONTROL_LABEL(CONTROL_LABELFILES, items);

  // set the filter label
  CStdString strLabel;

  // "Playlists"
  if (m_vecItems->m_strPath.Equals("special://musicplaylists/"))
    strLabel = g_localizeStrings.Get(136);
  // "{Playlist Name}"
  else if (m_vecItems->IsPlayList())
  {
    // get playlist name from path
    CStdString strDummy;
    CUtil::Split(m_vecItems->m_strPath, strDummy, strLabel);
  }
  // everything else is from a musicdb:// path
  else
  {
  }

  SET_CONTROL_LABEL(CONTROL_FILTER, strLabel);

  SET_CONTROL_SELECTED(GetID(),CONTROL_BTNPARTYMODE, g_partyModeManager.IsEnabled());

  SET_CONTROL_SELECTED(GetID(),CONTROL_BTN_FILTER, !m_filter.IsEmpty());
}

void CGUIWindowMusicNav::PlayItem(int iItem)
{
  // unlike additemtoplaylist, we need to check the items here
  // before calling it since the current playlist will be stopped
  // and cleared!

  // root is not allowed
  if (m_vecItems->IsVirtualDirectoryRoot())
    return;

  CGUIWindowMusicBase::PlayItem(iItem);
}

void CGUIWindowMusicNav::OnWindowLoaded()
{
#ifdef PRE_SKIN_VERSION_2_1_COMPATIBILITY
  const CGUIControl *pList = GetControl(CONTROL_LIST);
  if (pList && !GetControl(CONTROL_LABELEMPTY))
  {
    CLabelInfo info;
    info.align = XBFONT_CENTER_X | XBFONT_CENTER_Y;
    info.font = g_fontManager.GetFont("font13");
    info.textColor = 0xffffffff;
    CGUILabelControl *pLabel = new CGUILabelControl(GetID(),CONTROL_LABELEMPTY,pList->GetXPosition(),pList->GetYPosition(),pList->GetWidth(),pList->GetHeight(),info,false,false);
    pLabel->SetAnimations(pList->GetAnimations());
    Add(pLabel);
  }
#endif

  CGUIWindowMusicBase::OnWindowLoaded();
}

void CGUIWindowMusicNav::GetContextButtons(int itemNumber, CContextButtons &buttons)
{
}

bool CGUIWindowMusicNav::OnContextButton(int itemNumber, CONTEXT_BUTTON button)
{
  CFileItemPtr item;
  if (itemNumber >= 0 && itemNumber < m_vecItems->Size())
    item = m_vecItems->Get(itemNumber);

  switch (button)
  {
  case CONTEXT_BUTTON_INFO:
    {
      if (!item->IsVideoDb())
        return CGUIWindowMusicBase::OnContextButton(itemNumber,button);
      if (item->m_strPath.Left(14).Equals("videodb://3/4/"))
      {
        long idArtist = m_musicdatabase.GetArtistByName(item->GetLabel());
        if (idArtist == -1)
          return false;
        item->m_strPath.Format("musicdb://2/%ld/", m_musicdatabase.GetArtistByName(item->GetLabel()));
        CGUIWindowMusicBase::OnContextButton(itemNumber,button);
        Update(m_vecItems->m_strPath);
        m_viewControl.SetSelectedItem(itemNumber);
        return true;
      }
      CGUIWindowVideoNav* pWindow = (CGUIWindowVideoNav*)m_gWindowManager.GetWindow(WINDOW_VIDEO_NAV);
      if (pWindow)
      {
        SScraperInfo info;
        pWindow->OnInfo(item,info);
        Update(m_vecItems->m_strPath);
      }
      return true;
    }

  case CONTEXT_BUTTON_INFO_ALL:
    OnInfoAll(itemNumber);
    return true;

  case CONTEXT_BUTTON_SET_ARTIST_THUMB:
  case CONTEXT_BUTTON_SET_PLUGIN_THUMB:
    SetThumb(itemNumber, button);
    return true;

  case CONTEXT_BUTTON_UPDATE_LIBRARY:
    {
      return true;
    }

  case CONTEXT_BUTTON_SET_DEFAULT:
    g_settings.m_defaultMusicLibSource = GetQuickpathName(item->m_strPath);
    g_settings.Save();
    return true;

  case CONTEXT_BUTTON_CLEAR_DEFAULT:
    g_settings.m_defaultMusicLibSource.Empty();
    g_settings.Save();
    return true;

  case CONTEXT_BUTTON_GO_TO_ARTIST:
    {
      CStdString strPath;
      CVideoDatabase database;
      database.Open();
      strPath.Format("videodb://3/4/%ld/",database.GetMatchingMusicVideo(item->GetMusicInfoTag()->GetArtist()));
      m_gWindowManager.ActivateWindow(WINDOW_VIDEO_NAV,strPath);
      return true;
    }

  case CONTEXT_BUTTON_PLAY_OTHER:
    {
      CVideoDatabase database;
      database.Open();
      CVideoInfoTag details;
      database.GetMusicVideoInfo("",details,database.GetMatchingMusicVideo(item->GetMusicInfoTag()->GetArtist(),item->GetMusicInfoTag()->GetAlbum(),item->GetMusicInfoTag()->GetTitle()));
      g_application.getApplicationMessenger().PlayFile(CFileItem(details));
      return true;
    }

  case CONTEXT_BUTTON_MARK_WATCHED:
    CGUIWindowVideoBase::MarkWatched(item);
    CUtil::DeleteVideoDatabaseDirectoryCache();
    Update(m_vecItems->m_strPath);
    return true;

  case CONTEXT_BUTTON_MARK_UNWATCHED:
    CGUIWindowVideoBase::MarkUnWatched(item);
    CUtil::DeleteVideoDatabaseDirectoryCache();
    Update(m_vecItems->m_strPath);
    return true;

  case CONTEXT_BUTTON_RENAME:
    CGUIWindowVideoBase::UpdateVideoTitle(item.get());
    CUtil::DeleteVideoDatabaseDirectoryCache();
    Update(m_vecItems->m_strPath);
    return true;

  case CONTEXT_BUTTON_DELETE:
    CGUIWindowVideoNav::DeleteItem(item.get());
    CUtil::DeleteVideoDatabaseDirectoryCache();
    Update(m_vecItems->m_strPath);
    return true;

  case CONTEXT_BUTTON_SET_CONTENT:
    {
      bool bScan=false;
      SScraperInfo info;
      if (!m_musicdatabase.GetScraperForPath(item->m_strPath,info))
        info.strContent = "albums";

      int iLabel=132;
      // per genre or for all artists
      if (m_vecItems->m_strPath.Equals("musicdb://1/") || item->m_strPath.Equals("musicdb://2/"))
      {
        iLabel = 133;
      }

      if (CGUIDialogContentSettings::Show(info, bScan,iLabel))
      {
        m_musicdatabase.SetScraperForPath(item->m_strPath,info);
        if (bScan)
          OnInfoAll(itemNumber,true);
      }
      return true;
    }

  default:
    break;
  }

  return CGUIWindowMusicBase::OnContextButton(itemNumber, button);
}

void CGUIWindowMusicNav::SetThumb(int iItem, CONTEXT_BUTTON button)
{
  CFileItemPtr pItem = m_vecItems->Get(iItem);
  CFileItemList items;
  CStdString picturePath;
  CStdString strPath=pItem->m_strPath;
  CStdString strThumb;
  CStdString cachedThumb;

  if (button == CONTEXT_BUTTON_SET_ARTIST_THUMB)
  {
    long idArtist = -1;
    if (pItem->IsMusicDb())
    {
      CUtil::RemoveSlashAtEnd(strPath);
      int nPos=strPath.ReverseFind("/");
      if (nPos>-1)
      {
        //  try to guess where the user should start
        //  browsing for the artist thumb
        idArtist=atol(strPath.Mid(nPos+1));
      }
    }
    else if (pItem->IsVideoDb())
      idArtist = m_musicdatabase.GetArtistByName(pItem->GetLabel());

    m_musicdatabase.GetArtistPath(idArtist, picturePath);

    cachedThumb = pItem->GetCachedArtistThumb();

    CArtist artist;
    m_musicdatabase.GetArtistInfo(idArtist,artist);
    int i=1;
    for (std::vector<CScraperUrl::SUrlEntry>::iterator iter=artist.thumbURL.m_url.begin();iter != artist.thumbURL.m_url.end();++iter)
    {
      CStdString thumbFromWeb;
      CStdString strLabel;
      strLabel.Format("allmusicthumb%i.jpg",i);
      CUtil::AddFileToFolder("z:\\", strLabel, thumbFromWeb);
      if (CScraperUrl::DownloadThumbnail(thumbFromWeb,*iter))
      {
        CStdString strItemPath;
        strItemPath.Format("thumb://Remote%i",i++);
        CFileItemPtr item(new CFileItem(strItemPath, false));
        item->SetThumbnailImage(thumbFromWeb);
        CStdString strLabel;
        item->SetLabel(g_localizeStrings.Get(20015));
        items.Add(item);
      }
    }
  }
  else
  {
    strPath = m_vecItems->Get(iItem)->m_strPath;
    strPath.Replace("plugin://music/","Q:\\plugins\\music\\");
    strPath.Replace("/","\\");
    picturePath = strPath;
    CFileItem item(strPath,true);
    cachedThumb = item.GetCachedProgramThumb();
  }

  if (XFILE::CFile::Exists(cachedThumb))
  {
    CFileItemPtr item(new CFileItem("thumb://Current", false));
    item->SetThumbnailImage(cachedThumb);
    item->SetLabel(g_localizeStrings.Get(20016));
    items.Add(item);
  }

  if (button == CONTEXT_BUTTON_SET_PLUGIN_THUMB)
  {
    if (items.Size() == 0)
    {
      CFileItem item2(strPath,false);
      CUtil::AddFileToFolder(strPath,"default.py",item2.m_strPath);
      if (XFILE::CFile::Exists(item2.GetCachedProgramThumb()))
      {
        CFileItemPtr item(new CFileItem("thumb://Current", false));
        item->SetThumbnailImage(item2.GetCachedProgramThumb());
        item->SetLabel(g_localizeStrings.Get(20016));
        items.Add(item);
      }
    }

    CUtil::AddFileToFolder(strPath,"default.tbn",strThumb);
    if (XFILE::CFile::Exists(strThumb))
    {
      CFileItemPtr item(new CFileItem(strThumb,false));
      item->SetThumbnailImage(strThumb);
      item->SetLabel(g_localizeStrings.Get(20017));
      items.Add(item);
    }
  }

  CUtil::AddFileToFolder(picturePath,"folder.jpg",strThumb);
  if (XFILE::CFile::Exists(strThumb))
  {
    CFileItemPtr pItem(new CFileItem(strThumb,false));
    pItem->SetLabel(g_localizeStrings.Get(20017));
    pItem->SetThumbnailImage(strThumb);
    items.Add(pItem);
  }

  CFileItemPtr nItem(new CFileItem("thumb://None",false));
  nItem->SetLabel(g_localizeStrings.Get(20018));
  if (button == CONTEXT_BUTTON_SET_ARTIST_THUMB)
    nItem->SetThumbnailImage("DefaultArtistBig.png");
  else
    nItem->SetThumbnailImage("DefaultFolderBig.png");
  items.Add(nItem);

  if (CGUIDialogFileBrowser::ShowAndGetImage(items, g_settings.m_musicSources,
                                             g_localizeStrings.Get(20019), picturePath))
  {
    CPicture picture;
    if (picturePath.Equals("thumb://Current"))
      return;

    if (picturePath.Equals("thumb://None"))
    {
      XFILE::CFile::Delete(cachedThumb);
      if (button == CONTEXT_BUTTON_SET_PLUGIN_THUMB)
      {
        CPicture picture;
        picture.CacheSkinImage("DefaultFolderBig.png",cachedThumb);
        CFileItem item2(strPath,false);
        CUtil::AddFileToFolder(strPath,"default.py",item2.m_strPath);
        XFILE::CFile::Delete(item2.GetCachedProgramThumb());
      }
    }
    else if (button == CONTEXT_BUTTON_SET_PLUGIN_THUMB)
      XFILE::CFile::Cache(picturePath,cachedThumb);

    if (!picturePath.Equals("thumb://None") && picturePath.Left(8).Equals("thumb://") && items.Get(picturePath))
      picturePath = items.Get(picturePath)->GetThumbnailImage();

    if (picturePath.Equals("thumb://None") ||
        picture.DoCreateThumbnail(picturePath, cachedThumb))
    {
    }
    else
      CLog::Log(LOGERROR, " %s Could not cache artist/plugin thumb: %s", __FUNCTION__, picturePath.c_str());
  }
}

bool CGUIWindowMusicNav::GetSongsFromPlayList(const CStdString& strPlayList, CFileItemList &items)
{
  CStdString strParentPath=m_history.GetParentPath();

  if (m_guiState.get() && !m_guiState->HideParentDirItems())
  {
    CFileItemPtr pItem(new CFileItem(".."));
    pItem->m_strPath = strParentPath;
    items.Add(pItem);
  }

  items.m_strPath=strPlayList;
  CLog::Log(LOGDEBUG,"CGUIWindowMusicNav, opening playlist [%s]", strPlayList.c_str());

  auto_ptr<CPlayList> pPlayList (CPlayListFactory::Create(strPlayList));
  if ( NULL != pPlayList.get())
  {
    // load it
    if (!pPlayList->Load(strPlayList))
    {
      CGUIDialogOK::ShowAndGetInput(6, 0, 477, 0);
      return false; //hmmm unable to load playlist?
    }
    CPlayList playlist = *pPlayList;
    // convert playlist items to songs
    for (int i = 0; i < (int)playlist.size(); ++i)
    {
      items.Add(playlist[i]);
    }
  }

  return true;
}

void CGUIWindowMusicNav::DisplayEmptyDatabaseMessage(bool bDisplay)
{
  m_bDisplayEmptyDatabaseMessage = bDisplay;
}

void CGUIWindowMusicNav::Render()
{
  if (m_bDisplayEmptyDatabaseMessage)
  {
    SET_CONTROL_LABEL(CONTROL_LABELEMPTY,g_localizeStrings.Get(745)+'\n'+g_localizeStrings.Get(746))
  }
  else
  {
    SET_CONTROL_LABEL(CONTROL_LABELEMPTY,"")
  }
  CGUIWindowMusicBase::Render();
}

void CGUIWindowMusicNav::ClearFileItems()
{
  m_viewControl.Clear();
  m_vecItems->Clear();
  m_unfilteredItems->Clear();
}

void CGUIWindowMusicNav::OnFilterItems()
{
  CStdString currentItem;
  int item = m_viewControl.GetSelectedItem();
  if (item >= 0)
    currentItem = m_vecItems->Get(item)->m_strPath;

  m_viewControl.Clear();

  FilterItems(*m_vecItems);

  // and update our view control + buttons
  m_viewControl.SetItems(*m_vecItems);
  m_viewControl.SetSelectedItem(currentItem);
  UpdateButtons();
}

void CGUIWindowMusicNav::FilterItems(CFileItemList &items)
{
#if 0
  if (m_vecItems->IsVirtualDirectoryRoot())
    return;

  items.ClearItems();

  CStdString filter(m_filter);
  filter.TrimLeft().ToLower();
  bool numericMatch = StringUtils::IsNaturalNumber(filter);

  for (int i = 0; i < m_unfilteredItems->Size(); i++)
  {
    CFileItemPtr item = m_unfilteredItems->Get(i);
    if (item->IsParentFolder() || filter.IsEmpty() ||
        CMusicDatabaseDirectory::IsAllItem(item->m_strPath))
    {
      items.Add(item);
      continue;
    }
    // TODO: Need to update this to get all labels, ideally out of the displayed info (ie from m_layout and m_focusedLayout)
    // though that isn't practical.  Perhaps a better idea would be to just grab the info that we should filter on based on
    // where we are in the library tree.
    // Another idea is tying the filter string to the current level of the tree, so that going deeper disables the filter,
    // but it's re-enabled on the way back out.
    CStdString match;
/*    if (item->GetFocusedLayout())
      match = item->GetFocusedLayout()->GetAllText();
    else if (item->GetLayout())
      match = item->GetLayout()->GetAllText();
    else*/
    match = item->GetLabel();

    if (numericMatch)
      StringUtils::WordToDigits(match);
    size_t pos = StringUtils::FindWords(match.c_str(), filter.c_str());

    if (pos != CStdString::npos)
      items.Add(item);
  }
#endif
}

void CGUIWindowMusicNav::OnFinalizeFileItems(CFileItemList &items)
{
  CGUIMediaWindow::OnFinalizeFileItems(items);
  m_unfilteredItems->Append(items);
  // now filter as necessary
  if (!m_filter.IsEmpty())
    FilterItems(items);
}
