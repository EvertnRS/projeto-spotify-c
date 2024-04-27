#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reproduction
{
    int idUser;
    int idMusica;
    int idAlbum;
    long int timestamp;
    struct Reproduction *next;
    struct Reproduction *previous;
} Reproduction;

typedef struct UserMusicReport
{
    int idUser;
    int idMusica;
    int idAlbum;
    int numPlays;
    struct UserMusicReport *next;
    struct UserMusicReport *previous;
} UserMusicReport;

typedef struct MusicReport
{
    int idUser;
    int idMusica;
    int numPlays;
    struct MusicReport *next;
    struct MusicReport *previous;
} MusicReport;

typedef struct AlbumReport
{
    int idUser;
    int idAlbum;
    int numPlays;
    struct AlbumReport *next;
    struct AlbumReport *previous;
} AlbumReport;

void readFile(Reproduction **playlist, int *currentIndex);

UserMusicReport *countMusic(Reproduction *playlist);
void writeMusicReport(UserMusicReport *list);
void swapUserMusic(UserMusicReport *a, UserMusicReport *b);
void sortUserMusicReports(UserMusicReport *reportList);

MusicReport *mostPlayedMusicCheck(Reproduction *playlist);
void writeMostPlayedMusicReport(MusicReport *list);
int countMusicForUser(Reproduction *playlist, int userId, int musicId);
MusicReport *removeFromMusicList(MusicReport *head, MusicReport *nodeToRemove);
void swapMusic(MusicReport *a, MusicReport *b);
void sortMusicReports(MusicReport *reportList);

AlbumReport *mostPlayedAlbumCheck(Reproduction *playlist);
void writeMostPlayedAlbumReport(AlbumReport *list);
int countAlbumForUser(Reproduction *playlist, int userId, int albumId);
AlbumReport *removeFromAlbumList(AlbumReport *head, AlbumReport *nodeToRemove);
void swapAlbum(AlbumReport *a, AlbumReport *b);
void sortAlbumReports(AlbumReport *reportList);

void freeReproductionList(Reproduction *playlist);
void freeUserMusicReportList(UserMusicReport *list);
void freeMusicReportList(MusicReport *list);
void freeAlbumReportList(AlbumReport *list);

int main(void)
{
    Reproduction *playlist = NULL;
    int menu_option;
    int currentIndex = 0, *pcurrentIndex = &currentIndex;
    while (1)
    {
        printf("\n -=-=-=-=-=- MENU -=-=-=-=-=-\n [1] - Realizar Carga\n [0] - Sair do Programa\n -=-=-=-=-=--=-=-=-=-=-=-=-=-\n Digite sua entrada: ");
        scanf("%d", &menu_option);
        while(getchar() != '\n');

        switch (menu_option)
        {
        case 0:
            printf("Finalizando Spotifeio...");
            freeReproductionList(playlist);
            exit(0);
        case 1:
            readFile(&playlist, pcurrentIndex);
            UserMusicReport *musicList = countMusic(playlist);
            writeMusicReport(musicList);
            MusicReport *mostPlayedMusicList = mostPlayedMusicCheck(playlist);
            writeMostPlayedMusicReport(mostPlayedMusicList);
            AlbumReport *mostPlayedAlbumList = mostPlayedAlbumCheck(playlist);
            writeMostPlayedAlbumReport(mostPlayedAlbumList);
            
            freeReproductionList(playlist);
            playlist = NULL;
            freeUserMusicReportList(musicList);
            musicList = NULL;
            freeMusicReportList(mostPlayedMusicList);
            mostPlayedMusicList = NULL;
            freeAlbumReportList(mostPlayedAlbumList);
            mostPlayedAlbumList = NULL;
            break;
        default:
            printf("Entrada Inválida");
            break;
        }
        currentIndex = 0;
    }
}

void readFile(Reproduction **playlist, int *currentIndex)
{
    char fileName[50], lineSize[100];
    printf("Digite o nome do arquivo a ser lido: ");
    fgets(fileName, sizeof(fileName), stdin);
    char *p;
    if ((p = strchr(fileName, '\n')) != NULL)
    {
        *p = '\0';
    }

    char load[100];
    strcpy(load, "../");
    strcat(load, fileName);

    FILE *file = fopen(load, "r");
    if (file != NULL)
    {
        while (!feof(file))
        {
            Reproduction *newMusic = malloc(sizeof(Reproduction));
            if (*currentIndex == 0)
            {
                fgets(lineSize, 100, file);
            }
            fscanf(
                file, "%d;%d;%d;%ld;\n", &newMusic->idUser,
                &newMusic->idMusica, &newMusic->idAlbum,
                &newMusic->timestamp);
            (*currentIndex)++;
            
            newMusic->next = NULL;
            if (*playlist == NULL)
            {
                *playlist = newMusic;
            }
            else
            {
                Reproduction *currentMusic = *playlist;
                while (currentMusic->next != NULL)
                {
                    currentMusic = currentMusic->next;
                }
                currentMusic->next = newMusic;
            }
        }
        fclose(file);
    }
    else
    {
        printf("Falha ao abrir o arquivo\n");
    }
    printf("Musicas adicionadas com sucesso\n");
}

UserMusicReport *countMusic(Reproduction *playlist) {
    UserMusicReport *reportList = NULL;

    while (playlist != NULL) {
        int currentUserId = playlist->idUser;
        int currentMusicId = playlist->idMusica;
        int currentAlbumId = playlist->idAlbum;
        int count = 0;
        int alreadyCounted = 0; // Flag para verificar se já contou essa combinação

        // Verificar se essa combinação já foi contada anteriormente
        UserMusicReport *temp = reportList;
        while (temp != NULL) {
            if (temp->idUser == currentUserId && temp->idMusica == currentMusicId) {
                alreadyCounted = 1;
                break;
            }
            temp = temp->next;
        }

        // Se essa combinação ainda não foi contada, conte
        if (!alreadyCounted) {
            Reproduction *current = playlist;
            while (current != NULL) {
                if (current->idUser == currentUserId && current->idMusica == currentMusicId) {
                    count++;
                }
                current = current->next;
            }

            // Cria um novo nó no relatório
            UserMusicReport *newReport = malloc(sizeof(UserMusicReport));
            newReport->idUser = currentUserId;
            newReport->idMusica = currentMusicId;
            newReport->idAlbum = currentAlbumId;
            newReport->numPlays = count;
            newReport->next = NULL;

            // Adiciona o relatório à lista
            if (reportList == NULL) {
                reportList = newReport;
            } else {
                UserMusicReport *last = reportList;
                while (last->next != NULL) {
                    last = last->next;
                }
                last->next = newReport;
            }
        }

        // Avança para o próximo nó da lista
        while (playlist != NULL && playlist->idUser == currentUserId) {
            playlist = playlist->next;
        }
    }

    sortUserMusicReports(reportList);

    return reportList;
}

void writeMusicReport(UserMusicReport *list)
{
    FILE *pontRel = fopen("usuario_musica.csv", "w");
    if (pontRel != NULL)
    {
        fprintf(pontRel, "idUser;idMusica;idAlbum;numPlays\n");

        while (list != NULL)
        {
            fprintf(pontRel, "%d;%d;%d;%d\n", list->idUser, list->idMusica, list->idAlbum, list->numPlays);
            list = list->next;
        }

        fclose(pontRel);
    }
    else
    {
        puts("Não foi possível criar o arquivo.");
    }
}

void swapUserMusic(UserMusicReport *a, UserMusicReport *b) {
    int tempUsuario = a->idUser;
    int tempMusica = a->idMusica;
    int tempAlbum = a->idAlbum;
    int tempPlays = a->numPlays;

    a->idUser = b->idUser;
    a->idMusica = b->idMusica;
    a->idAlbum = b->idAlbum;
    a->numPlays = b->numPlays;

    b->idUser = tempUsuario;
    b->idMusica = tempMusica;
    b->idAlbum = tempAlbum;
    b->numPlays = tempPlays;
}

void sortUserMusicReports(UserMusicReport *reportList) {
    int swapped;
    UserMusicReport *ptr1;
    UserMusicReport *lptr = NULL;

    if (reportList == NULL || reportList->next == NULL)
        return;

    do {
        swapped = 0;
        ptr1 = reportList;

        while (ptr1->next != lptr) {
            if (ptr1->idUser < ptr1->next->idUser) {
                swapUserMusic(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

MusicReport *mostPlayedMusicCheck(Reproduction *playlist) {
    MusicReport *reportList = NULL;

    while (playlist != NULL) {
        int currentUserId = playlist->idUser;
        int currentMusicId = playlist->idMusica;

        // Procura na lista se já existe um relatório para esse usuário
        MusicReport *existingReport = NULL;
        MusicReport *current = reportList;
        while (current != NULL) {
            if (current->idUser == currentUserId) {
                existingReport = current;
                break;
            }
            current = current->next;
        }

        // Se não houver relatório existente, ou a música atual é mais reproduzida, cria ou atualiza o relatório
        if (!existingReport || existingReport->numPlays < countMusicForUser(playlist, currentUserId, currentMusicId)) {
            // Remove o relatório antigo, se existir
            if (existingReport) {
                reportList = removeFromMusicList(reportList, existingReport);
                free(existingReport);
            }

            // Cria um novo relatório para a música mais reproduzida
            MusicReport *newReport = malloc(sizeof(MusicReport));
            newReport->idUser = currentUserId;
            newReport->idMusica = currentMusicId;
            newReport->numPlays = countMusicForUser(playlist, currentUserId, currentMusicId);
            newReport->next = NULL;

            if (reportList == NULL) {
                reportList = newReport;
            } else {
                newReport->next = reportList;
                reportList = newReport;
            }
        }

        playlist = playlist->next;
    }

    sortMusicReports(reportList);

    return reportList;
}

void writeMostPlayedMusicReport(MusicReport *list)
{
    FILE *pontRel = fopen("rel_musicas.csv", "w");
    if (pontRel != NULL)
    {
        fprintf(pontRel, "idUser;idMusica;numPlays\n");
        while (list != NULL)
        {
            fprintf(pontRel, "%d;%d;%d\n", list->idUser, list->idMusica, list->numPlays);
            list = list->next;
        }
        fclose(pontRel);
    }
    else
    {
        puts("Não foi possível criar o arquivo.");
    }
}

int countMusicForUser(Reproduction *playlist, int userId, int musicId) {
    int count = 0;
    while (playlist != NULL) {
        if (playlist->idUser == userId && playlist->idMusica == musicId) {
            count++;
        }
        playlist = playlist->next;
    }
    return count;
}

MusicReport *removeFromMusicList(MusicReport *head, MusicReport *nodeToRemove) {
    if (head == NULL || nodeToRemove == NULL) {
        return head;
    }

    if (head == nodeToRemove) {
        MusicReport *newHead = head->next;
        free(head);
        return newHead;
    }

    MusicReport *current = head;
    while (current->next != NULL && current->next != nodeToRemove) {
        current = current->next;
    }

    // Remover o nó
    if (current->next == nodeToRemove) {
        current->next = nodeToRemove->next;
        free(nodeToRemove);
    }

    return head;
}

void swapMusic(MusicReport *a, MusicReport *b) {
    int tempUsuario = a->idUser;
    int tempMusica = a->idMusica;
    int tempPlays = a->numPlays;

    a->idUser = b->idUser;
    a->idMusica = b->idMusica;
    a->numPlays = b->numPlays;

    b->idUser = tempUsuario;
    b->idMusica = tempMusica;
    b->numPlays = tempPlays;
}

void sortMusicReports(MusicReport *reportList) {
    int swapped;
    MusicReport *ptr1;
    MusicReport *lptr = NULL;

    if (reportList == NULL || reportList->next == NULL)
        return;

    do {
        swapped = 0;
        ptr1 = reportList;

        while (ptr1->next != lptr) {
            if (ptr1->idUser < ptr1->next->idUser) {
                swapMusic(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

//-------------------------------------------------------------------------------

AlbumReport *mostPlayedAlbumCheck(Reproduction *playlist) {
    AlbumReport *reportList = NULL;

    while (playlist != NULL) {
        int currentUserId = playlist->idUser;
        int currentAlbumId = playlist->idAlbum;

        // Procura na lista se já existe um relatório para esse usuário
        AlbumReport *existingReport = NULL;
        AlbumReport *current = reportList;
        while (current != NULL) {
            if (current->idUser == currentUserId) {
                existingReport = current;
                break;
            }
            current = current->next;
        }

        // Se não houver relatório existente, ou o album atual é mais reproduzido, cria ou atualiza o relatório
        if (!existingReport || existingReport->numPlays < countAlbumForUser(playlist, currentUserId, currentAlbumId)) {
            // Remove o relatório antigo, se existir
            if (existingReport) {
                reportList = removeFromAlbumList(reportList, existingReport);
                free(existingReport);
            }

            // Cria um novo relatório para a música mais reproduzida
            AlbumReport *newReport = malloc(sizeof(AlbumReport));
            newReport->idUser = currentUserId;
            newReport->idAlbum = currentAlbumId;
            newReport->numPlays = countAlbumForUser(playlist, currentUserId, currentAlbumId);
            newReport->next = NULL;

            // Adiciona o novo relatório à lista
            if (reportList == NULL) {
                reportList = newReport;
            } else {
                newReport->next = reportList;
                reportList = newReport;
            }
        }

        // Avança para o próximo nó da lista
        playlist = playlist->next;
    }

    sortAlbumReports(reportList);

    return reportList;
}

void writeMostPlayedAlbumReport(AlbumReport *list)
{
    FILE *pontRel = fopen("rel_album.csv", "w");
    if (pontRel != NULL)
    {
        fprintf(pontRel, "idUser;idAlbum;numPlays\n");
        while (list != NULL)
        {
            fprintf(pontRel, "%d;%d;%d\n", list->idUser, list->idAlbum, list->numPlays);
            list = list->next;
        }
        fclose(pontRel);
    }
    else
    {
        puts("Não foi possível criar o arquivo.");
    }
}

int countAlbumForUser(Reproduction *playlist, int userId, int albumId) {
    int count = 0;
    while (playlist != NULL) {
        if (playlist->idUser == userId && playlist->idAlbum == albumId) {
            count++;
        }
        playlist = playlist->next;
    }
    return count;
}

AlbumReport *removeFromAlbumList(AlbumReport *head, AlbumReport *nodeToRemove) {
    if (head == NULL || nodeToRemove == NULL) {
        return head;
    }

    if (head == nodeToRemove) {
        AlbumReport *newHead = head->next;
        free(head);
        return newHead;
    }

    AlbumReport *current = head;
    while (current->next != NULL && current->next != nodeToRemove) {
        current = current->next;
    }

    if (current->next == nodeToRemove) {
        current->next = nodeToRemove->next;
        free(nodeToRemove);
    }

    return head;
}

void swapAlbum(AlbumReport *a, AlbumReport *b) {
    int tempUsuario = a->idUser;
    int tempAlbum = a->idAlbum;
    int tempPlays = a->numPlays;

    a->idUser = b->idUser;
    a->idAlbum = b->idAlbum;
    a->numPlays = b->numPlays;

    b->idUser = tempUsuario;
    b->idAlbum = tempAlbum;
    b->numPlays = tempPlays;
}

// Função para ordenar a lista de relatórios por número de reproduções (Bubble Sort)
void sortAlbumReports(AlbumReport *reportList) {
    int swapped;
    AlbumReport *ptr1;
    AlbumReport *lptr = NULL;

    // Caso a lista esteja vazia ou tenha apenas um nó, ela já está ordenada
    if (reportList == NULL || reportList->next == NULL)
        return;

    do {
        swapped = 0;
        ptr1 = reportList;

        while (ptr1->next != lptr) {
            if (ptr1->idUser < ptr1->next->idUser) {
                swapAlbum(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

//--------------------------------------------------------------

void freeReproductionList(Reproduction *playlist) {
    Reproduction *current = playlist;
    while (current != NULL) {
        Reproduction *temp = current;
        current = current->next;
        free(temp);
    }
}

void freeUserMusicReportList(UserMusicReport *list) {
    UserMusicReport *current = list;
    while (current != NULL) {
        UserMusicReport *temp = current;
        current = current->next;
        free(temp);
    }
}

void freeMusicReportList(MusicReport *list) {
    MusicReport *current = list;
    while (current != NULL) {
        MusicReport *temp = current;
        current = current->next;
        free(temp);
    }
}

void freeAlbumReportList(AlbumReport *list) {
    AlbumReport *current = list;
    while (current != NULL) {
        AlbumReport *temp = current;
        current = current->next;
        free(temp);
    }
}