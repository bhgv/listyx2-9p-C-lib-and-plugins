
/*
 * Copyright bhgv 2017
 */

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "ltm.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <lib9.h>
#include "styxserver.h"
#include "styx.h"

#include "lstyx.h"




#ifdef DEBUG
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif



#define BUF_SIZE 1024


char*
ls_dir_rd_out(const char *path, Qid qid, char *buf, ulong *n, vlong *off){
	Dir d;
	int m = 0;
	int dsz = 0;

	int dri = *off;

	int i;
	
	DIR *dir = NULL;
	struct dirent *ent;
	
	char *name_buf = NULL;

	struct stat file_info;
	int status = 0;
	
	char* tmp_nm;
	
	name_buf = malloc(BUF_SIZE);
	if( !name_buf )
		return NULL;
	
	memset( &d, 0, sizeof(Dir) );

DBG("%s:%d (%x:%x) path = %s\n\n", __func__, __LINE__, (int)(qid.path>>32), (int)qid.path, path);
//	d.uid = eve;
//	d.gid = eve;
	
	// Open directory
	dir = opendir(path);
	if (!dir) {
		*n = 0;
		free(name_buf);
		return nil;
	}
	
	// Read entries
	i = 0; m = 0;
	while ((ent = readdir(dir)) != NULL) {
		//d.qid.path = (1<<17) + i;
		
		d.length = 0;
		d.qid.my_type = FS_FILE_DIR;
		
		d.qid.type = QTDIR;
		d.mode = DMDIR;

		//type = 'd';
		if (ent->d_type == DT_REG) {
			d.qid.my_type = FS_FILE_FILE;
			//type = 'f';
//DBG("%s:%d max(%d): %s/%s\t", __func__, __LINE__, path, ent->d_name);
			snprintf(name_buf, BUF_SIZE-1, "%s/%s", path, ent->d_name);

			status = stat(name_buf, &file_info);
			d.length = file_info.st_size;
DBG("%s:%d fl_name=%s, fl_len=%d\n", __func__, __LINE__, name_buf, d.length);

			d.qid.type = QTFILE;
 			d.mode = DMREAD | DMWRITE | DMEXEC;

//			sprintf(size,"%8d", ent->d_fsize);
		}

		//tmp_nm = malloc( strlen(ent->d_name) + 1);
		//strcpy(tmp_nm, ent->d_name);
		d.name = ent->d_name; //tmp_nm;

		if(i >= dri){
			dsz = convD2M(&d, (uchar*)buf, *n-m);
			if(dsz <= BIT16SZ)
					break;
			m += dsz;
			buf += dsz;
		}
	
		i++;
	}
	closedir(dir);
	
	*n = m;
	*off = i;

	free(name_buf);
	
	return nil;
}



#if 1
Path
make_file_path(Path new_type, Path oldp, int idx){
	Path newp = 0ULL;
DBG("\n%s:%d oldp = %x:%x, i = %d\n", __func__, __LINE__, (int)(oldp>>32), (int)oldp, idx);
	Path lvl = PATH_LVL_GET(oldp);
DBG("%s:%d lvl = %x:%x\n", __func__, __LINE__, (int)(lvl>>32), (int)lvl);

	Path mainpt = oldp & PATH_MAIN_PT_MASK;
DBG("%s:%d mainpt = %x:%x\n", __func__, __LINE__, (int)(mainpt>>32), (int)mainpt);

	Path curpt = idx & PATH_STEP_MASK;
	curpt <<= lvl * PATH_MAIN_PT_STEP;
DBG("%s:%d curpt = %x:%x\n", __func__, __LINE__, (int)(curpt>>32), (int)curpt);

	mainpt &= ~( PATH_STEP_MASK << (lvl * PATH_MAIN_PT_STEP) );
DBG("%s:%d mainpt = %x:%x\n", __func__, __LINE__, (int)(mainpt>>32), (int)mainpt);
	mainpt |= curpt;
DBG("%s:%d mainpt = %x:%x\n", __func__, __LINE__, (int)(mainpt>>32), (int)mainpt);

	lvl++;
DBG("%s:%d lvl = %x:%x\n", __func__, __LINE__, (int)(lvl>>32), (int)lvl);

	PATH_LVL_SET(newp, lvl);
DBG("%s:%d newp = %x:%x\n", __func__, __LINE__, (int)(newp>>32), (int)newp);
	newp |= mainpt;
DBG("%s:%d newp = %x:%x\n", __func__, __LINE__, (int)(newp>>32), (int)newp);
DBG("%s:%d new_type = %x:%x\n", __func__, __LINE__, (int)(new_type>>32), (int)new_type);
	PATH_TYPE_COPY(newp, new_type);
DBG("%s:%d newp = %x:%x\n\n", __func__, __LINE__, (int)(newp>>32), (int)newp);
	
	return newp;
}
#endif



/*
Path
make_file_path(Path new_type, Path oldp, int idx){
	Path newp = 0ULL;
//DBG("\n%s:%d oldp = %x:%x, i = %d\n", __func__, __LINE__, (int)(oldp>>32), (int)oldp, idx);
	Path lvl = PATH_LVL_GET(oldp);
//DBG("%s:%d lvl = %x:%x\n", __func__, __LINE__, (int)(lvl>>32), (int)lvl);

	Path mainpt = oldp & PATH_MAIN_PT_MASK;
//DBG("%s:%d mainpt = %x:%x\n", __func__, __LINE__, (int)(mainpt>>32), (int)mainpt);

	Path curpt = idx & PATH_STEP_MASK;
	curpt <<= lvl * PATH_MAIN_PT_STEP;
//DBG("%s:%d curpt = %x:%x\n", __func__, __LINE__, (int)(curpt>>32), (int)curpt);

	mainpt &= ~( PATH_STEP_MASK << (lvl * PATH_MAIN_PT_STEP) );
//DBG("%s:%d mainpt = %x:%x\n", __func__, __LINE__, (int)(mainpt>>32), (int)mainpt);
	mainpt |= curpt;
//DBG("%s:%d mainpt = %x:%x\n", __func__, __LINE__, (int)(mainpt>>32), (int)mainpt);

	lvl++;
//DBG("%s:%d lvl = %x:%x\n", __func__, __LINE__, (int)(lvl>>32), (int)lvl);

	PATH_LVL_SET(newp, lvl);
//DBG("%s:%d newp = %x:%x\n", __func__, __LINE__, (int)(newp>>32), (int)newp);
	newp |= mainpt;
//DBG("%s:%d newp = %x:%x\n", __func__, __LINE__, (int)(newp>>32), (int)newp);
//DBG("%s:%d new_type = %x:%x\n", __func__, __LINE__, (int)(new_type>>32), (int)new_type);
	PATH_TYPE_COPY(newp, new_type);
//DBG("%s:%d newp = %x:%x\n\n", __func__, __LINE__, (int)(newp>>32), (int)newp);
	
	return newp;
}
*/

char*
scan_fs_dir(Qid *qid, const char *path, char *nm){
	int i;
	char *r = "No exists";
	
	DIR *dir = NULL;
	struct dirent *ent;
	
	// Open directory
	dir = opendir(path);
	if (!dir) {
		return r;
	}
	
	// Read entries
	i = 0; 
DBG("\n%s:%d. path=%s nm=%s old_path=%x:%x\n\n", __func__, __LINE__, path, nm, (int)(qid->path >> 32), (int)qid->path );
	while ((ent = readdir(dir)) != NULL) {
		int l;
		char *cur_nm = ent->d_name;
		l = strlen(cur_nm); //ent->d_namlen;
		if((l == 1 && cur_nm[0] == '.') || (l == 2 && cur_nm[0] == '.' && cur_nm[1] == '.'))
			continue;
		
		if( !strcmp(ent->d_name, nm) ){
//			qid->path = PATH_FILE + (PATH_STEP_MASK & i);
			qid->path = make_file_path( PATH_FILE, (Path)qid->path, i);
DBG("\n%s:%d new_path = %x:%x\n\n", __func__, __LINE__, (int)(qid->path >> 32), (int)qid->path );
			
			//d.length = 0;
			qid->my_type = FS_FILE_DIR;
			qid->type = QTDIR;
			//d.mode = DMDIR;

			//type = 'd';
			if (ent->d_type == DT_REG) {
				qid->my_type = FS_FILE_FILE;
				qid->type = QTFILE;
				//d.mode = DMDIR;

	//			sprintf(size,"%8d", ent->d_fsize);
			}

			qid->my_name = strdup(ent->d_name);

			r = nil;
			break;
		}
		i++;
	}
	closedir(dir);
	
	return r;
}



int
_aux_file_pathname_from_path(Path path, Path lvl, Path mainpt, char* buf, char* pbuf, int cur_len, int max_len){
	int i;
		
	DIR *dir = NULL;
	struct dirent *ent;

DBG("%s:%d path = %x:%x\n", __func__, __LINE__, (int)(path>>32), (int)path);

//	Path lvl = PATH_LVL_GET(path);
//	Path mainpt = path & PATH_MAIN_PT_MASK;

//	char *pthnm = "/";
//	int cur_len = 0;
	char *cur_nm;

//	char *pbuf = buf;

	if(lvl < 0)
		return cur_len;

//for( ; lvl > 0; lvl--)
//{
	int cur_n = (int)(mainpt & PATH_STEP_MASK);
	mainpt >>= PATH_MAIN_PT_STEP;

	cur_len++;
	if(cur_len > max_len) return 0;
	
	*pbuf = '/'; pbuf++; *pbuf = '\0';

DBG("%s:%d path = %s\n", __func__, __LINE__, buf );
	// Open directory
	dir = opendir( buf );
	if (!dir) {
		return 0;
	}
	
	// Read entries
	i = 0; 
DBG("\n%s:%d lvl = %d, cur_n = %d\n", __func__, __LINE__, lvl, cur_n );
	while ((ent = readdir(dir)) != NULL) {
		int l;
		
		cur_nm = ent->d_name;
		l = strlen(cur_nm); //ent->d_namlen;
		if((l == 1 && cur_nm[0] == '.') || (l == 2 && cur_nm[0] == '.' && cur_nm[1] == '.'))
			continue;
			
DBG("%s:%d i=%d ?= cur_n=%d\n", __func__, __LINE__, i, cur_n);
		if( i == cur_n ){
DBG("%s:%d cur_nm = %s, nm_len = %d, %d\n", __func__, __LINE__, cur_nm, l, strlen(cur_nm) );
			if (ent->d_type == DT_REG) {
				if( lvl > 1){
DBG("\nERROR! %s:%d. lvl = %d, but fn = %s is a file!\n\n", __func__, __LINE__, lvl, cur_nm);
					return 0;
				}
				//d.length = ent->d_fsize;
			}
			
			cur_len += l;
			if(cur_len > max_len){
				closedir(dir);
				return 0;
			}
			
			memcpy(pbuf, cur_nm, l);
			pbuf += l;
			*pbuf = '\0';
			
			break;
		}
		i++;
	}
	closedir(dir);
	
	lvl--;
	if(/*i == cur_n &&*/ (int)lvl > 0){
DBG("%s:%d lvl = %d\n", __func__, __LINE__, lvl );
		cur_len = _aux_file_pathname_from_path(path, lvl, mainpt, buf, pbuf, cur_len, max_len);
	}
//}

	return cur_len;
}



int
file_pathname_from_path(Path path, char* buf, int max_len){
	int i;
		
	DIR *dir = NULL;
	struct dirent *ent;

DBG("%s:%d path = %x:%x\n", __func__, __LINE__, (int)(path>>32), (int)path);

	Path lvl = PATH_LVL_GET(path);
	Path mainpt = path & PATH_MAIN_PT_MASK;

//	char *pthnm = "/";
	int cur_len = 0;
	char *cur_nm;

	char *pbuf = buf;

cur_len = _aux_file_pathname_from_path(path, lvl, mainpt, buf, buf, 0, max_len);
DBG("file_pathname_from_path cur_len=%d, path = %s\n", cur_len, buf );

#if 0
for( ; lvl > 0; lvl--){
	int cur_n = (int)(mainpt & PATH_STEP_MASK);
	mainpt >>= PATH_MAIN_PT_STEP;

	cur_len++;
	if(cur_len > max_len) return 0;
	
	*pbuf = '/';
	pbuf++;
	*pbuf = '\0';

//DBG("file_pathname_from_path path = %s\n", buf );
	// Open directory
	dir = opendir( buf );
	if (!dir) {
		return 0;
	}
	
	// Read entries
	i = 0; 
//DBG("\nfile_pathname_from_path lvl = %d, cur_n = %d\n", lvl, cur_n );
	while ((ent = readdir(dir)) != NULL) {
		if( i == cur_n ){
			int l;
			
			cur_nm = ent->d_name;
			l = strlen(cur_nm); //ent->d_namlen;
//DBG("file_pathname_from_path cur_nm = %s, nm_len = %d, %d\n", cur_nm, l, strlen(cur_nm) );
			if (ent->d_type == DT_REG) {
				if( lvl > 1){
//DBG("\nERROR! file_pathname_from_path. lvl = %d, but fn = %s is a file!\n\n", lvl, cur_nm);
					return 0;
				}
				//d.length = ent->d_fsize;
			}
			
			cur_len += l;
			if(cur_len > max_len){
				closedir(dir);
				return 0;
			}
			
			memcpy(pbuf, cur_nm, l);
			pbuf += l;
			*pbuf = '\0';
			
			break;
		}
		i++;
	}
	closedir(dir);
}
#endif
	return cur_len;
}


